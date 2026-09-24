#ifndef _DTS_SHAPE_EXPORT_HPP
#define _DTS_SHAPE_EXPORT_HPP

#include <variant>

#include "dts/material/materials.hpp"
#include "dts/mesh/mesh.hpp"
#include "dts/sequence/sequence.hpp"
#include "dts/shape/detail.hpp"
#include "io/io.hpp"
#include "io/write.hpp"

namespace dts::shape {

auto write_integer_set(io::ostream &stream, const sequence::IntegerSet &set)
    -> void {
   io::write<i32>(stream, 0);
   io::write<i32>(stream, set.size());
   io::write(stream, set);
}

auto write_sequence(io::ostream &stream, const sequence::Sequence &sequence)
    -> void {
   io::write(stream, sequence.name_index);
   io::write(stream, sequence.flags);
   io::write(stream, sequence.key_frame_count);
   io::write(stream, sequence.duration);
   io::write(stream, sequence.priority);
   io::write(stream, sequence.ground_frame_start);
   io::write(stream, sequence.ground_frame_count);
   io::write(stream, sequence.base_rotation);
   io::write(stream, sequence.base_translation);
   io::write(stream, sequence.base_scale);
   io::write(stream, sequence.base_object_state);
   io::write(stream, 0);
   io::write(stream, sequence.trigger_start);
   io::write(stream, sequence.trigger_count);
   io::write(stream, sequence.tool_begin);

   write_integer_set(stream, sequence.rotation_matters);
   write_integer_set(stream, sequence.translation_matters);
   write_integer_set(stream, sequence.scale_matters);

   write_integer_set(stream, sequence::IntegerSet{});
   write_integer_set(stream, sequence::IntegerSet{});

   write_integer_set(stream, sequence.vis_matters);
   write_integer_set(stream, sequence.frame_matters);
   write_integer_set(stream, sequence.mat_frame_matters);
}

auto write_detail(io::ostream &stream, const shape::Detail &detail,
                  const i32 version) -> void {
   io::write(stream, detail.name_index);
   io::write(stream, detail.sub_shape_num);
   io::write(stream, detail.object_detail_num);
   io::write(stream, detail.size);
   io::write(stream, detail.average_error);
   io::write(stream, detail.max_error);
   io::write(stream, detail.polycount);
   // NOLINTBEGIN(readability-magic-numbers)
   if (version >= 26) {
      io::write(stream, detail.bb_dimension);
      io::write(stream, detail.bb_detail_level);
      io::write(stream, detail.bb_equator_steps);
      io::write(stream, detail.bb_polar_steps);
      io::write(stream, detail.bb_polar_angle);
      io::write(stream, detail.bb_include_poles);
   }
   // NOLINTEND(readability-magic-numbers)
}

auto write_materials(io::ostream &stream, const material::Materials &materials,
                     const i32 version) -> void {
   constexpr auto VALID_MATERIAL_VERSION = 1;

   io::write<u8>(stream, materials.version);
   if (materials.version != VALID_MATERIAL_VERSION) {
      return;
   }

   io::write<u32>(stream, materials.material_names.size());
   if (materials.material_names.empty()) {
      return;
   }

   for (const auto &name : materials.material_names) {
      io::write<u8>(stream, name.size());
      io::write(stream, name);
   }
   io::write(stream, materials.flags);
   io::write(stream, materials.reflectance_maps);
   io::write(stream, materials.bump_maps);
   io::write(stream, materials.detail_maps);
   io::write(stream, materials.detail_scales);
   io::write(stream, materials.reflection_amounts);
}

auto write_mesh(io::WriteGuard &guard, const mesh::Mesh &mesh,
                const i32 version) -> void {
   guard.guard();

   io::write(guard.stream32, mesh.frame_count);
   io::write(guard.stream32, mesh.mat_frame_count);
   io::write(guard.stream32, mesh.parent_mesh);
   io::write(guard.stream32, mesh.bounds);
   io::write(guard.stream32, mesh.center);
   io::write<i32>(guard.stream32, mesh.radius);

   io::write<i32>(guard.stream32, mesh.vertices.size());
   if (mesh.parent_mesh < 0) {
      io::write(guard.stream32, mesh.vertices);
      io::write<i32>(guard.stream32, mesh.tangents.size());
      io::write(guard.stream32, mesh.tangents);
   } else {
      io::write(guard.stream32, 0);
      // NOLINTBEGIN(readability-magic-numbers)
      if (version > 25) {
         io::write(guard.stream32, 0);
         io::write(guard.stream32, 0);
      }
      // NOLINTEND(readability-magic-numbers)
   }

   // NOLINTBEGIN(readability-magic-numbers)
   if (mesh.parent_mesh < 0 && version > 25) {
      io::write<i32>(guard.stream32, mesh.tangents2.size());
      io::write(guard.stream32, mesh.tangents2);
      io::write<i32>(guard.stream32, mesh.vertex_colors.size());
      io::write(guard.stream32, mesh.vertex_colors);
   }
   // NOLINTEND(readability-magic-numbers)

   if (mesh.parent_mesh < 0) {
      io::write(guard.stream32, mesh.normals);
      for (auto i = 0; i < mesh.normals.size(); ++i) {
         io::write<i8>(guard.stream8, 0);
      }
   }

   // NOLINTBEGIN(readability-magic-numbers)
   if (version > 25) {
      io::write<i32>(guard.stream32, mesh.primitives.size());
      io::write(guard.stream32, mesh.primitives);
      io::write<i32>(guard.stream32, mesh.indices.size());
      io::write(guard.stream32, mesh.indices);
   } else {
      throw std::runtime_error(
          "mesh primitives and indices are not supported in version " +
          std::to_string(version));
   }
   // NOLINTEND(readability-magic-numbers)

   io::write(guard.stream32, 0);
   io::write(guard.stream32, mesh.verts_per_frame);
   io::write(guard.stream32, mesh.flags);

   guard.guard();
}

auto write_mesh(io::WriteGuard &guard, const mesh::SkinnedMesh &mesh,
                const i32 version) -> void {
   write_mesh(guard, mesh.mesh, version);

   io::write<i32>(guard.stream32, mesh.initial_vertices.size());
   if (mesh.mesh.parent_mesh < 0) {
      io::write(guard.stream32, mesh.initial_vertices);
      io::write(guard.stream32, mesh.initial_normals);
      for (auto i = 0; i < mesh.initial_vertices.size(); ++i) {
         io::write<i8>(guard.stream8, 0);
      }
   }

   io::write<i32>(guard.stream32, mesh.initial_transforms.size());
   if (mesh.mesh.parent_mesh < 0) {
      io::write(guard.stream32, mesh.initial_transforms);
   }

   io::write<i32>(guard.stream32, mesh.vertex_indices.size());
   if (mesh.mesh.parent_mesh < 0) {
      io::write(guard.stream32, mesh.vertex_indices);
      io::write(guard.stream32, mesh.bone_indices);
      io::write(guard.stream32, mesh.weights);
   }

   io::write<i32>(guard.stream32, mesh.node_indices.size());
   if (mesh.mesh.parent_mesh < 0) {
      io::write(guard.stream32, mesh.node_indices);
   }

   guard.guard();
}

auto write_mesh(io::WriteGuard &guard, const mesh::MeshType &mesh,
                const i32 version) -> void {
   if (!mesh.has_value()) {
      io::write(guard.stream32, 4);
      return;
   }
   std::visit(
       [&guard, version](auto &mesh_unwrapped) -> void {
          using ValueType = std::remove_reference<decltype(mesh_unwrapped)>;
          io::write<i32>(guard.stream32, ValueType::type::TYPE);
          write_mesh(guard, mesh_unwrapped, version);
       },
       *mesh);
}

} // namespace dts::shape

#endif // _DTS_SHAPE_EXPORT_HPP
