#ifndef _DTS_IO_STREAM_HPP
#define _DTS_IO_STREAM_HPP

#include "dts/collections.hpp"
#include "io/io.hpp"

namespace dts::io {
class VectorBuffer : public io::stream_buf {
 public:
   explicit VectorBuffer(const usize size) {
      this->buffer.resize(size);
      io::stream_buf::setp(this->buffer.data(), this->buffer.data() + size);
   }

   auto data() -> const std::byte * { return this->buffer.data(); }
   auto size() -> usize { return this->buffer.size(); }

 protected:
   auto overflow(int_type c) -> int_type override {
      if (traits_type::eq_int_type(c, traits_type::eof())) {
         return traits_type::not_eof(c);
      }

      const auto pos = pptr() ? static_cast<usize>(pptr() - pbase()) : 0;

      this->buffer.resize(this->buffer.size() + 1024);

      this->setp(this->buffer.data(),
                 this->buffer.data() + this->buffer.size());
      this->pbump(pos);
      *pptr() = traits_type::to_char_type(c);
      this->pbump(1);
      return c;
   }

   auto seekoff(off_type off, std::ios_base::seekdir dir,
                std::ios_base::openmode which = std::ios_base::in |
                                                std::ios_base::out)
       -> pos_type override {
      if (!(which & std::ios_base::out)) {
         return pos_type(off_type(-1));
      }

      auto current = static_cast<off_type>(pptr() - pbase());

      off_type target;

      switch (dir) {
      case std::ios_base::beg:
         target = off;
         break;

      case std::ios_base::cur:
         target = current + off;
         break;

      case std::ios_base::end:
         target = static_cast<off_type>(buffer.size()) + off;
         break;

      default:
         return pos_type(off_type(-1));
      }

      if (target < 0) {
         return pos_type(off_type(-1));
      }

      if (static_cast<usize>(target) > buffer.size()) {
         buffer.resize(static_cast<usize>(target));
      }

      this->setp(buffer.data(), buffer.data() + buffer.size());

      pbump(static_cast<int>(target));

      return pos_type(target);
   }

 private:
   collections::Vec<std::byte> buffer;
};
} // namespace dts::io

#endif // _DTS_IO_STREAM_HPP
