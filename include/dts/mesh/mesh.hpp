#ifndef _DTS_MESH_MESH_HPP
#define _DTS_MESH_MESH_HPP

#include <optional>
#include <variant>

#include "dts/collections.hpp"
#include "dts/dts.hpp"
#include "dts/types.hpp"

namespace dts::mesh {

using namespace types;
using namespace collections;

struct DTS_API Primitive final {
   i32 start{0};
   i32 count{0};
   i32 material_index{0};
};

///
struct DTS_API Mesh final {
 public:
   static constexpr u8 TYPE = 0;

   i32 parent_mesh{0};
   i32 frame_count{0};
   i32 mat_frame_count{0};
   vec6 bounds;
   vec3 center;
   f32 radius{0};
   Vec<vec3> vertices;
   Vec<vec2> tangents;
   Vec<vec2> tangents2;
   Vec<u8vec4> vertex_colors;
   Vec<vec3> normals;
   Vec<Primitive> primitives;
   Vec<i32> indices;
   i32 verts_per_frame{0};
   u32 flags{0};
   bool has_vert_2{false};
   bool has_color{false};
};

///
struct DTS_API SkinnedMesh final {
 public:
   static constexpr u8 TYPE = 1;

   Mesh mesh;
   Vec<vec3> initial_vertices;
   Vec<vec3> initial_normals;
   Vec<mat4x4> initial_transforms;
   Vec<i32> vertex_indices;
   Vec<i32> bone_indices;
   Vec<f32> weights;
   Vec<i32> node_indices;
};

using MeshType = std::optional<std::variant<Mesh, SkinnedMesh>>;

} // namespace dts::mesh

#endif // _DTS_MESH_MESH_HPP
