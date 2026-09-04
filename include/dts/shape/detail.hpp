#ifndef _DTS_SHAPE_DETAIL_HPP
#define _DTS_SHAPE_DETAIL_HPP

#include <type_traits>

#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::shape {

using namespace types;

///
struct DTS_API Detail final {
 public:
   i32 name_index{0};
   i32 sub_shape_num{0};
   i32 object_detail_num{0};
   f32 size{0};
   f32 average_error{0};
   f32 max_error{0};
   i32 polycount{0};
   i32 bb_dimension{0};
   i32 bb_detail_level{0};
   i32 bb_equator_steps{0};
   i32 bb_polar_steps{0};
   f32 bb_polar_angle{0};
   i32 bb_include_poles{0};
};
static_assert(std::is_trivially_copyable_v<Detail>);

} // namespace dts::shape

#endif // _DTS_SHAPE_DETAIL_HPP
