#ifndef _DTS_MATERIAL_MATERIALS_HPP
#define _DTS_MATERIAL_MATERIALS_HPP

#include <string>

#include "dts/collections.hpp"
#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::material {

using namespace types;
using namespace collections;

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
