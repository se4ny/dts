#ifndef _DTS_SHAPE_IMPORT_HPP
#define _DTS_SHAPE_IMPORT_HPP

#include "dts/material/materials.hpp"
#include "dts/mesh/mesh.hpp"
#include "dts/sequence/sequence.hpp"
#include "dts/shape/detail.hpp"
#include "dts/types.hpp"
#include "io/io.hpp"
#include "io/read.hpp"

namespace dts::shape::io {

using namespace dts::io;
using namespace collections;

using MeshType = std::optional<std::variant<mesh::Mesh, mesh::SkinnedMesh>>;

auto read_materials(io::istream &stream, const i32 version)
    -> std::optional<material::Materials> {

   auto mat_version = io::read_unchecked_fix_endianess<u8>(stream);
   if (mat_version != 1) {
      return std::nullopt;
   }

   auto mat_count = io::read_unchecked_fix_endianess<u32>(stream);
   if (mat_count == 0) {
      return std::nullopt;
   }

   Vec<std::u8string> mat_names;
   mat_names.reserve(mat_count);
   for (auto i = 0; i < mat_count; ++i) {
      auto name_len = io::read_unchecked_fix_endianess<u8>(stream);
      auto name = std::u8string{};
      name.resize(name_len);
      stream.read(reinterpret_cast<std::byte *>(name.data()), name_len);

      mat_names.push_back(name);
   }

   auto flags = io::read_unchecked<u32>(stream, mat_count);
   auto reflectance_maps = io::read_unchecked<u32>(stream, mat_count);
   auto bump_maps = io::read_unchecked<u32>(stream, mat_count);
   auto detail_maps = io::read_unchecked<u32>(stream, mat_count);

   if (version == 25) {
      stream.seekg(sizeof(u32) * mat_count, std::ios::cur);
   }

   auto detail_scales = io::read_unchecked<f32>(stream, mat_count);
   auto reflection_amounts = io::read_unchecked<f32>(stream, mat_count);

   // return material::Materials{};
   return material::Materials{
       .version = mat_version,
       .material_names = std::move(mat_names),
       .flags = std::move(flags),
       .reflectance_maps = std::move(reflectance_maps),
       .bump_maps = std::move(bump_maps),
       .detail_maps = std::move(detail_maps),
       .detail_scales = std::move(detail_scales),
       .reflection_amounts = std::move(reflection_amounts),
   };
}

auto read_integer_set(io::istream &stream) -> sequence::IntegerSet {
   const auto _ = io::read_unchecked<i32>(stream);

   const auto length = io::read_unchecked<i32>(stream);
   sequence::IntegerSet result{0};
   for (auto i = 0; i < length; ++i) {
      result[i] = io::read_unchecked<i32>(stream);
   }
   return result;
}

auto read_sequence(io::istream &stream) -> sequence::Sequence {
   auto sequence = sequence::Sequence{
       .name_index = io::read_unchecked<u32>(stream),
       .flags = io::read_unchecked<i32>(stream),
       .key_frame_count = io::read_unchecked<i32>(stream),
       .duration = io::read_unchecked<f32>(stream),
       .priority = io::read_unchecked<i32>(stream),
       .ground_frame_start = io::read_unchecked<i32>(stream),
       .ground_frame_count = io::read_unchecked<i32>(stream),
       .base_rotation = io::read_unchecked<i32>(stream),
       .base_translation = io::read_unchecked<i32>(stream),
       .base_scale = io::read_unchecked<i32>(stream),
       .base_object_state = io::read_unchecked<i32>(stream),
   };

   io::seek_unchecked<i32>(stream, 1);

   sequence.trigger_start = io::read_unchecked<i32>(stream);
   sequence.trigger_count = io::read_unchecked<i32>(stream);
   sequence.tool_begin = io::read_unchecked<f32>(stream);

   sequence.rotation_matters = read_integer_set(stream);
   sequence.translation_matters = read_integer_set(stream);
   sequence.scale_matters = read_integer_set(stream);

   read_integer_set(stream);
   read_integer_set(stream);

   sequence.vis_matters = read_integer_set(stream);
   sequence.frame_matters = read_integer_set(stream);
   sequence.mat_frame_matters = read_integer_set(stream);

   return sequence;
}

auto read_detail(io::istream &stream, i32 version) -> Detail {
   auto detail = Detail{
       .name_index = io::read_unchecked<i32>(stream),
       .sub_shape_num = io::read_unchecked<i32>(stream),
       .object_detail_num = io::read_unchecked<i32>(stream),
       .size = io::read_unchecked<f32>(stream),
       .average_error = io::read_unchecked<f32>(stream),
       .max_error = io::read_unchecked<f32>(stream),
       .polycount = io::read_unchecked<i32>(stream),
   };
   if (version >= 26) {
      detail.bb_dimension = io::read_unchecked<i32>(stream);
      detail.bb_detail_level = io::read_unchecked<i32>(stream);
      detail.bb_equator_steps = io::read_unchecked<u32>(stream);
      detail.bb_polar_steps = io::read_unchecked<u32>(stream);
      detail.bb_polar_angle = io::read_unchecked<f32>(stream);
      detail.bb_include_poles = io::read_unchecked<u32>(stream);
   }
   return detail;
}

auto read_standard_mesh(io::istream &stream32, io::istream &stream16,
                        io::istream &stream8, i32 version) -> mesh::Mesh {

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto frame_count = io::read_unchecked<i32>(stream32);
   auto mat_frame_count = io::read_unchecked<i32>(stream32);
   auto parent_mesh = io::read_unchecked<i32>(stream32);

   auto bounds = io::read_unchecked<vec6>(stream32);
   auto center = io::read_unchecked<vec3>(stream32);
   auto radius = io::read_unchecked<i32>(stream32);

   mesh::Mesh mesh{
       .parent_mesh = parent_mesh,
       .frame_count = frame_count,
       .mat_frame_count = mat_frame_count,
       .bounds = bounds,
       .center = center,
       .radius = static_cast<f32>(radius),
   };

   auto vertex_count = io::read_unchecked<i32>(stream32);
   if (parent_mesh < 0) {
      mesh.vertices = io::read_unchecked<vec3>(stream32, vertex_count);
      auto vertex_tangent_count = io::read_unchecked<i32>(stream32);
      mesh.tangents = io::read_unchecked<vec2>(stream32, vertex_tangent_count);
   } else {
      io::seek_unchecked<i32>(stream32, 1);
      if (version > 25) {
         mesh.has_vert_2 = io::read_unchecked<i32>(stream32) != 0;
         mesh.has_color = io::read_unchecked<i32>(stream32) != 0;
      }
   }

   if (parent_mesh < 0 && version > 25) {
      auto vertex_tangent2_count = io::read_unchecked<i32>(stream32);
      mesh.tangents2 =
          io::read_unchecked<vec2>(stream32, vertex_tangent2_count);
      auto vertex_color_count = io::read_unchecked<i32>(stream32);
      mesh.vertex_colors =
          io::read_unchecked<u8vec4>(stream32, vertex_color_count);
   };

   if (parent_mesh < 0) {
      mesh.normals = io::read_unchecked<vec3>(stream32, vertex_count);

      io::seek_unchecked<i8>(stream8, vertex_count);
   }

   if (version > 25) {
      auto primitive_len = io::read_unchecked<i32>(stream32);
      mesh.primitives =
          io::read_unchecked<mesh::Primitive>(stream32, primitive_len);
      auto indices_len = io::read_unchecked<i32>(stream32);
      mesh.indices = io::read_unchecked<i32>(stream32, indices_len);
   } else {
      auto primitive_len = io::read_unchecked<i32>(stream32);
      auto primitive_adj_len = primitive_len * 2;
      auto prim16 = io::read_unchecked<i16>(stream16, primitive_adj_len);
      auto prim32 = io::read_unchecked<i32>(stream32, primitive_len);

      auto indices_len = io::read_unchecked<i32>(stream32);
      auto indices16 = io::read_unchecked<i16>(stream16, indices_len);

      mesh.primitives.reserve(primitive_len);
      for (auto i = 0; i < primitive_len; ++i) {
         mesh.primitives.emplace_back<mesh::Primitive>({
             .start = prim16[i * 2],
             .count = prim16[(i * 2) + 1],
             .material_index = prim32[i],
         });
      }
      mesh.indices = Vec<i32>(indices16.begin(), indices16.end());
   }

   auto len = io::read_unchecked<i32>(stream32);

   io::seek_unchecked<i16>(stream16, len);

   mesh.verts_per_frame = io::read_unchecked<i32>(stream32);
   mesh.flags = io::read_unchecked<u32>(stream32);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   return mesh;
}

auto read_skinned_mesh(io::istream &stream32, io::istream &stream16,
                       io::istream &stream8, i32 version) -> mesh::SkinnedMesh {

   mesh::SkinnedMesh skinned_mesh{
       .mesh = read_standard_mesh(stream32, stream16, stream8, version),
   };

   i32 vertex_count = io::read_unchecked<i32>(stream32);
   if (skinned_mesh.mesh.parent_mesh < 0) {
      skinned_mesh.intial_vertices =
          io::read_unchecked<vec3>(stream32, vertex_count);
      skinned_mesh.initial_normals =
          io::read_unchecked<vec3>(stream32, vertex_count);

      io::seek_unchecked<i8>(stream8, vertex_count);
   }

   auto length = io::read_unchecked<i32>(stream32);
   if (skinned_mesh.mesh.parent_mesh < 0) {
      skinned_mesh.initial_transforms =
          io::read_unchecked<mat4x4>(stream32, length);
   }

   length = io::read_unchecked<i32>(stream32);
   if (skinned_mesh.mesh.parent_mesh < 0) {
      auto vertex_indices = io::read_unchecked<i32>(stream32, length);
      auto bone_indices = io::read_unchecked<i32>(stream32, length);
      auto weights = io::read_unchecked<f32>(stream32, length);

      skinned_mesh.vertex_indices =
          Vec<i32>(vertex_indices.begin(), vertex_indices.end());
      skinned_mesh.bone_indices =
          Vec<i32>(bone_indices.begin(), bone_indices.end());
      skinned_mesh.weights = Vec<f32>(weights.begin(), weights.end());
   }

   length = io::read_unchecked<i32>(stream32);
   if (skinned_mesh.mesh.parent_mesh < 0) {
      auto node_indices = io::read_unchecked<i32>(stream32, length);

      skinned_mesh.node_indices =
          Vec<i32>(node_indices.begin(), node_indices.end());
   }

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   return skinned_mesh;
}

auto read_mesh(io::istream &stream32, io::istream &stream16,
               io::istream &stream8, i32 version) -> MeshType {

   i32 mesh_type = io::read_unchecked<i32>(stream32);
   switch (mesh_type) {
   case mesh::Mesh::TYPE:
      return read_standard_mesh(stream32, stream16, stream8, version);
   case mesh::SkinnedMesh::TYPE:
      return read_skinned_mesh(stream32, stream16, stream8, version);
   default:
      return std::nullopt;
   }
}

} // namespace dts::shape::io

#endif // _DTS_SHAPE_IMPORT_HPP
