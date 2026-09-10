#ifndef _DTS_SHAPE_OBJECT_HPP
#define _DTS_SHAPE_OBJECT_HPP

#include <type_traits>

#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::shape {

using namespace types;

///
struct DTS_API ObjectState final {
 public:
   f32 vis{0};
   i32 frame_index{0};
   i32 mat_frame_index{0};
};
static_assert(std::is_trivially_copyable_v<ObjectState>);

///
struct DTS_API Object final {
 public:
   i32 name_index{0};
   i32 mesh_count{0};
   i32 start_mesh_index{0};
   i32 node_index{0};
   i32 next_sibling{0};

 private:
   i32 _padding{0};
};
static_assert(std::is_trivially_copyable_v<Object>);

} // namespace dts::shape

#endif // _DTS_SHAPE_OBJECT_HPP
