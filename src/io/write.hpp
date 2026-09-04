#ifndef _DTS_IO_WRITE_HPP
#define _DTS_IO_WRITE_HPP

#include "dts/collections.hpp"
#include "io/io.hpp"

namespace dts::io {

template <typename T> auto write(io::ostream &stream, const T &val) -> void {
   stream.write(reinterpret_cast<const std::byte *>(&val), sizeof(T));
}

template <typename T>
auto write(io::ostream &stream, const collections::Vec<T> &sequence) -> void {
   stream.write(reinterpret_cast<const std::byte *>(sequence.data()),
                sequence.size() * sizeof(T));
}

template <typename T, usize N>
auto write(io::ostream &stream, const std::array<T, N> &sequence) -> void {
   stream.write(reinterpret_cast<const std::byte *>(sequence.data()),
                sequence.size() * sizeof(T));
}

template <typename T>
auto write(io::ostream &stream, const std::span<T> &sequence) -> void {
   stream.write(reinterpret_cast<const std::byte *>(sequence.data()),
                sequence.size() * sizeof(T));
}

auto write(io::ostream &stream, const std::u8string sequence) -> void {
   stream.write(reinterpret_cast<const std::byte *>(sequence.data()),
                sequence.size());
}

struct WriteGuard final {
   explicit WriteGuard(io::ostream &stream32, io::ostream &stream16,
                       io::ostream &stream8)
       : stream32(stream32), stream16(stream16), stream8(stream8) {}

   io::ostream &stream32;
   io::ostream &stream16;
   io::ostream &stream8;

   auto guard() -> void {
      io::write(stream32, this->pos32++);
      io::write<i16>(stream16, this->pos16++);
      io::write<i8>(stream8, this->pos8++);
   }

 private:
   u32 pos32{0};
   u32 pos16{0};
   u32 pos8{0};
};

} // namespace dts::io

#endif // _DTS_IO_WRITE_HPP
