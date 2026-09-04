//!

#ifndef _DTS_SHAPE_SHAPE_HPP
#define _DTS_SHAPE_SHAPE_HPP

#include <expected>
#include <filesystem>
#include <string>

#include "dts/collections.hpp"
#include "dts/dts.hpp"
#include "dts/material/materials.hpp"
#include "dts/mesh/mesh.hpp"
#include "dts/sequence/sequence.hpp"
#include "dts/shape/detail.hpp"
#include "dts/shape/node.hpp"
#include "dts/shape/object.hpp"
#include "dts/shape/trigger.hpp"
#include "dts/types.hpp"

namespace dts::shape {

using namespace types;
using namespace collections;

/// TODO
struct DTS_API Shape final {
 public:
   // Shape() = default;
   // Shape(const Shape &) = delete;

   // Shape(i32 version) : version{version} {}

   i32 version;

   material::Materials materials;
   Vec<Detail> details;
   Vec<Trigger> triggers;
   Vec<Object> objects;
   Vec<ObjectState> object_states;
   Vec<std::u8string> names;
   Vec<Node> nodes;
   Vec<sequence::Sequence> sequences;
   Vec<mesh::MeshType> meshes;

   i32 smallest_visible_size{0};
   i32 smallest_visible_dl{0};
   f32 radius{0};
   f32 radius_tube{0};
   vec3 center{};
   vec6 bounds{};

   Vec<i32> sub_shape_first_nodes;
   Vec<i32> sub_shape_first_objects;
   Vec<i32> sub_shape_nodes;
   Vec<i32> sub_shape_objects;
   //
   Vec<i16quat> default_rotations;
   Vec<vec3> default_translations;
   Vec<vec3> node_translations;
   Vec<i16quat> node_rotations;
   Vec<f32> node_uniform_scales;
   Vec<vec3> node_aligned_scales;
   Vec<vec3> node_arbitrary_scale_factors;
   Vec<i16quat> node_arbitrary_scale_rotations;
   Vec<vec3> ground_translations;
   Vec<i16quat> ground_rotations;
};

[[nodiscard]]
DTS_API auto import_shape(const std::filesystem::path &)
    -> std::expected<shape::Shape, std::string>;

[[nodiscard]]
DTS_API auto export_shape(const shape::Shape &, std::filesystem::path &)
    -> bool;

} // namespace dts::shape

#endif // _DTS_SHAPE_SHAPE_HPP
