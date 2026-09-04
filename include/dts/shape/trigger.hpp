#ifndef _DTS_SHAPE_TRIGGER_HPP
#define _DTS_SHAPE_TRIGGER_HPP

#include <type_traits>

#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::shape {

using namespace types;

///
enum DTS_API TriggerState : u32 {
   InvertOnReverse = 1U << 30,
   On = 1U << 31,
   Mask = (1U << 30) - 1,
};

///
struct DTS_API Trigger final {
 public:
   TriggerState state{On};
   f32 position{0};
};
static_assert(std::is_trivially_copyable_v<Trigger>);

} // namespace dts::shape

#endif // _DTS_SHAPE_TRIGGER_HPP
