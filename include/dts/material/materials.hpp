#ifndef _DTS_MATERIAL_MATERIALS_HPP
#define _DTS_MATERIAL_MATERIALS_HPP

#include <string>

#include "dts/collections.hpp"
#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::material {

using namespace types;
using namespace collections;

enum class DTS_API MaterialFlags {
   SWrap = 1 << 0,
   TWrap = 1 << 1,
   Translucent = 1 << 2,
   Additive = 1 << 3,
   Subtractive = 1 << 4,
   SelfIllumination = 1 << 5,
   NeverEnvMap = 1 << 6,
   NoMipMap = 1 << 7,
   MipMapZeroBorder = 1 << 8,
   AuxiliaryMap = 1 << 27 | 1 << 28 | 1 << 29 | 1 << 30 | 1 << 31,
};

struct DTS_API Materials final {
 public:
   u8 version{0};
   Vec<std::u8string> material_names;
   Vec<bool> was_mapped;
   Vec<u32> flags;
   Vec<u32> reflectance_maps;
   Vec<u32> bump_maps;
   Vec<u32> detail_maps;
   Vec<f32> detail_scales;
   Vec<f32> reflection_amounts;
};

} // namespace dts::material

#endif // _DTS_MATERIAL_MATERIALS_HPP
