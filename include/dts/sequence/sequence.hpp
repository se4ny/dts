#ifndef _DTS_SEQUENCE_SEQUENCE_HPP
#define _DTS_SEQUENCE_SEQUENCE_HPP

#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::sequence {

using namespace types;

constexpr auto MAX_INTEGER_SET_SIZE = 8;

using IntegerSet = std::array<i32, MAX_INTEGER_SET_SIZE>;

/// TODO
struct DTS_API Sequence final {
 public:
   u32 name_index{0};
   i32 flags{0};
   i32 key_frame_count{0};
   f32 duration{0};
   i32 priority{0};
   i32 ground_frame_start{0};
   i32 ground_frame_count{0};
   i32 base_rotation{0};
   i32 base_translation{0};
   i32 base_scale{0};
   i32 base_object_state{0};
   i32 trigger_start{0};
   i32 trigger_count{0};
   f32 tool_begin{0};

   IntegerSet rotation_matters{};
   IntegerSet translation_matters{};
   IntegerSet scale_matters{};
   IntegerSet vis_matters{};
   IntegerSet frame_matters{};
   IntegerSet mat_frame_matters{};
};

} // namespace dts::sequence

#endif // _DTS_SEQUENCE_SEQUENCE_HPP
