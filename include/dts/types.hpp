#ifndef _DTS_TYPES_HPP
#define _DTS_TYPES_HPP

#include <array>
#include <cstdint>

namespace dts::types {

using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

using usize = size_t;

#ifndef DTS_USE_GLM
template <typename T, usize N> using vec = std::array<T, N>;

using vec2 = vec<f32, 2>;
using vec3 = vec<f32, 3>;
using vec6 = vec<f32, 6>;
using u8vec4 = vec<u8, 4>;
using i16quat = vec<i16, 4>;
using mat4x4 = vec<f32, 16>;

#else
#endif

} // namespace dts::types

#endif // _DTS_TYPES_HPP
