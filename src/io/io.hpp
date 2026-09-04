#ifndef _DTS_IO_HPP
#define _DTS_IO_HPP

#include <fstream>
#include <spanstream>
#include <string>

#include "dts/types.hpp"

namespace dts::io {

using namespace types;

using ifstream = std::basic_ifstream<std::byte, std::char_traits<std::byte>>;
using spanstream =
    std::basic_spanstream<std::byte, std::char_traits<std::byte>>;
using ispanstream =
    std::basic_ispanstream<std::byte, std::char_traits<std::byte>>;
using istream = std::basic_istream<std::byte, std::char_traits<std::byte>>;
using ostream = std::basic_ostream<std::byte, std::char_traits<std::byte>>;
using ofstream = std::basic_ofstream<std::byte, std::char_traits<std::byte>>;
using stream_buf = std::basic_streambuf<std::byte, std::char_traits<std::byte>>;

template <typename T>
auto seek_unchecked(istream &stream, usize offset) -> void {
   stream.seekg(sizeof(T) * offset, std::ios::cur);
}

template <typename T> auto guard_unchecked(istream &stream) -> void {
   seek_unchecked<T>(stream, 1);
}

} // namespace dts::io

#endif // _DTS_IO_HPP
