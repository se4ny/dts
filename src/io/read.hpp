#ifndef _DTS_IO_READ_HPP
#define _DTS_IO_READ_HPP

#include <bit>
#include <type_traits>

#include "dts/collections.hpp"
#include "dts/types.hpp"
#include "io/io.hpp"

namespace dts::io {

using namespace types;
using namespace collections;

template <typename T>
   requires std::is_trivially_copyable_v<T>
auto read_unchecked(istream &stream) -> T {
   T val;
   stream.read(reinterpret_cast<std::byte *>(&val), sizeof val);
   return val;
}

template <typename T>
   requires std::is_trivially_copyable_v<T>
auto read_unchecked(istream &stream, usize range) -> Vec<T> {
   Vec<T> buffer;
   buffer.resize(range);
   stream.read(reinterpret_cast<std::byte *>(buffer.data()), range * sizeof(T));
   return buffer;
}

template <typename T>
   requires std::is_trivially_copyable_v<T>
auto read_unchecked_fix_endianess(istream &stream) -> T {
   T val = read_unchecked<T>(stream);
   if (std::endian::native == std::endian::little) [[likely]] {
      return val;
   }

   if constexpr (std::is_integral_v<T>) {
      return std::byteswap(val);
   } else {
      constexpr auto BIT64_SIZE = 8;
      using U = std::conditional_t<
          sizeof(T) == 4, std::uint32_t,
          std::conditional_t<sizeof(T) == BIT64_SIZE, std::uint64_t, void>>;

      static_assert(!std::is_same_v<U, void>, "Unsupported type size");

      U tmp = std::bit_cast<U>(val);
      tmp = std::byteswap(tmp);
      return std::bit_cast<T>(tmp);
   }
}

} // namespace dts::io

#endif // _DTS_IO_READ_HPP
