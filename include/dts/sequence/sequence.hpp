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
   // Corresponds to @ref Shape::names.
   u32 name_index{0};
   i32 flags{0};

   // Number of key frames in this sequence.
   i32 key_frame_count{0};

   // Duration of this sequence in seconds.
   f32 duration{0};

   i32 priority{0};
   i32 ground_frame_start{0};
   i32 ground_frame_count{0};

   // Corresponds to @ref Shape::base_rotations.
   i32 base_rotation{0};

   // Corresponds to @ref Shape::base_translations.
   i32 base_translation{0};

   i32 base_scale{0};
   i32 base_object_state{0};
   i32 trigger_start{0};
   i32 trigger_count{0};
   f32 tool_begin{0};

   // Defines which nodes' rotations are affected by this sequence.
   IntegerSet rotation_matters{};

   // Defines which nodes' translations are affected by this sequence.
   IntegerSet translation_matters{};

   // Defines which nodes' scales are affected by this sequence.
   IntegerSet scale_matters{};

   // Defines which nodes' visibility are affected by this sequence.
   IntegerSet vis_matters{};

   // Defines which nodes' frame matters are affected by this sequence.
   IntegerSet frame_matters{};

   // Defines which nodes' material frame matters are affected by this sequence.
   IntegerSet mat_frame_matters{};
};

} // namespace dts::sequence

#endif // _DTS_SEQUENCE_SEQUENCE_HPP
