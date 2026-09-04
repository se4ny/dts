#ifndef _DTS_SHAPE_NODE_HPP
#define _DTS_SHAPE_NODE_HPP

#include <type_traits>

#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::shape {

using namespace types;

///
struct DTS_API Node final {
 public:
   i32 name_index{0};
   i32 parent_index{0};
   i32 first_object{0};
   i32 first_child{0};
   i32 next_sibling{0};
};
static_assert(std::is_trivially_copyable_v<Node>);

} // namespace dts::shape

#endif // _DTS_SHAPE_NODE_HPP
