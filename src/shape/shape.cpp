
#include <cmath>
#include <cstddef>
#include <optional>
#include <ranges>
#include <span>
#include <string>

#include "dts/collections.hpp"
#include "dts/mesh/mesh.hpp"
#include "dts/shape/node.hpp"
#include "dts/shape/shape.hpp"
#include "io/io.hpp"
#include "io/read.hpp"
#include "io/stream.hpp"
#include "io/write.hpp"
#include "shape/shape_export.hpp"
#include "shape/shape_import.hpp"

namespace dts::shape {

using namespace types;
using namespace collections;

constexpr auto EXPORT_VERSION = 26;
constexpr auto MAGIC_SHIFT = 16;

auto export_shape(const Shape &shape, std::filesystem::path &path) -> bool {

   io::ofstream outstream{path, std::ios::binary};
   if (!outstream.is_open()) {
      return false;
   }

   io::write(outstream, EXPORT_VERSION | (EXPORT_VERSION << MAGIC_SHIFT));

   constexpr auto BUFFER32_BYTE_SIZE = 1024 * 1024;
   constexpr auto BUFFER16_BYTE_SIZE = 8192;
   constexpr auto BUFFER8_BYTE_SIZE = 8192;

   io::VectorBuffer buffer32{BUFFER32_BYTE_SIZE};
   io::VectorBuffer buffer16{BUFFER16_BYTE_SIZE};
   io::VectorBuffer buffer8{BUFFER8_BYTE_SIZE};

   io::ostream stream32{&buffer32};
   io::ostream stream16{&buffer16};
   io::ostream stream8{&buffer8};

   io::write<i32>(stream32, shape.nodes.size());
   io::write<i32>(stream32, shape.objects.size());
   io::write<i32>(stream32, 0);
   io::write<i32>(stream32, shape.sub_shape_nodes.size());
   io::write<i32>(stream32, 0);
   io::write<i32>(stream32, shape.node_rotations.size());
   io::write<i32>(stream32, shape.node_translations.size());
   io::write<i32>(stream32, shape.node_uniform_scales.size());
   io::write<i32>(stream32, shape.node_aligned_scales.size());
   io::write<i32>(stream32, shape.node_arbitrary_scale_factors.size());
   io::write<i32>(stream32, shape.ground_rotations.size());
   io::write<i32>(stream32, shape.object_states.size());
   io::write<i32>(stream32, 0);
   io::write<i32>(stream32, shape.triggers.size());
   io::write<i32>(stream32, shape.details.size());
   io::write<i32>(stream32, shape.meshes.size());
   io::write<i32>(stream32, shape.names.size());
   io::write<i32>(stream32, shape.smallest_visible_size);
   io::write<i32>(stream32, shape.smallest_visible_dl);

   io::WriteGuard guard{stream32, stream16, stream8};

   guard.guard();

   io::write(stream32, shape.radius);
   io::write(stream32, shape.radius_tube);
   io::write(stream32, shape.center);
   io::write(stream32, shape.bounds);

   guard.guard();

   io::write(stream32, shape.nodes);

   guard.guard();

   io::write(stream32, shape.objects);

   guard.guard();
   guard.guard();
   guard.guard();

   io::write(stream32, shape.sub_shape_first_nodes);
   io::write(stream32, shape.sub_shape_first_objects);
   io::write<i32>(stream32, shape.sub_shape_nodes.size());

   guard.guard();

   io::write(stream32, shape.sub_shape_nodes);
   io::write(stream32, shape.sub_shape_objects);
   io::write<i32>(stream32, shape.sub_shape_nodes.size());

   guard.guard();

   io::write(stream16, shape.default_rotations);
   io::write(stream32, shape.default_translations);
   io::write(stream32, shape.node_translations);
   io::write(stream16, shape.node_rotations);

   guard.guard();

   io::write(stream32, shape.node_uniform_scales);
   io::write(stream32, shape.node_aligned_scales);
   io::write(stream32, shape.node_arbitrary_scale_factors);
   io::write(stream32, shape.node_arbitrary_scale_rotations);

   guard.guard();

   io::write(stream32, shape.ground_translations);
   io::write(stream16, shape.ground_rotations);

   guard.guard();

   io::write(stream32, shape.object_states);

   guard.guard();
   guard.guard();

   io::write(stream32, shape.triggers);

   guard.guard();

   for (const auto &detail : shape.details) {
      shape::write_detail(stream32, detail, EXPORT_VERSION);
   }

   guard.guard();

   for (const auto &mesh : shape.meshes) {
      shape::write_mesh(guard, mesh, EXPORT_VERSION);
   }

   guard.guard();

   for (const auto &name : shape.names) {
      io::write(stream8, name);
      io::write<u8>(stream8, '\0');
   }

   guard.guard();

   u64 pos32 = stream32.tellp();
   u64 pos16 = stream16.tellp();
   u64 pos8 = stream8.tellp();

   auto size32 = std::ceil(static_cast<f32>(pos32) / sizeof(i32));
   auto size16 = std::ceil(static_cast<f32>(pos16) / sizeof(i32));
   auto size8 = std::ceil(static_cast<f32>(pos8) / sizeof(i32));

   auto padding16 = ((pos16 + 3) & ~3) - pos16;
   for (auto i = 0; i < padding16; ++i) {
      io::write<i8>(stream16, 0);
   }

   auto padding8 = ((pos8 + 3) & ~3) - pos8;
   for (auto i = 0; i < padding8; ++i) {
      io::write<i8>(stream8, 0);
   }

   io::write<u32>(outstream, size32 + size16 + size8);
   io::write<u32>(outstream, size32);
   io::write<u32>(outstream, size32 + size16);

   outstream.flush();

   io::write(outstream, std::span{buffer32.data(), pos32});
   io::write(outstream, std::span{buffer16.data(), pos16 + padding16});
   io::write(outstream, std::span{buffer8.data(), pos8 + padding8});

   io::write<i32>(outstream, shape.sequences.size());
   for (const auto &sequence : shape.sequences) {
      shape::write_sequence(outstream, sequence);
   }

   shape::write_materials(outstream, shape.materials, EXPORT_VERSION);

   outstream.flush();

   return outstream.fail();
}

auto import_shape(const std::filesystem::path &path)
    -> std::expected<shape::Shape, std::string> {
   if (!std::filesystem::exists(path)) {
      return std::unexpected{"file not found"};
   }

   io::ifstream instream{path, std::ios::binary};
   if (!instream.is_open()) {
      return std::unexpected{"failed to open file"};
   }

   const auto version = io::read_unchecked_fix_endianess<i32>(instream);
   const auto real_version = version & 0xFF;
   const auto export_version = version >> MAGIC_SHIFT;

   const auto buffer_len = io::read_unchecked_fix_endianess<u32>(instream);
   const auto buffer16_beg = io::read_unchecked_fix_endianess<u32>(instream);
   const auto buffer8_beg = io::read_unchecked_fix_endianess<u32>(instream);

   const auto buffer16_len = (buffer8_beg - buffer16_beg) * sizeof(u16);
   const auto buffer8_len = (buffer_len - buffer8_beg) * sizeof(u32);

   const auto buff32 =
       io::read_unchecked<std::byte>(instream, buffer16_beg * sizeof(i32));
   const auto buff16 =
       io::read_unchecked<std::byte>(instream, buffer16_len * sizeof(i16));
   const auto buff8 =
       io::read_unchecked<std::byte>(instream, buffer8_len * sizeof(i8));

   // TODO: Fix endianess here.

   io::ispanstream stream32{buff32};
   io::ispanstream stream16{buff16};
   io::ispanstream stream8{buff8};

   i32 sequence_count = io::read_unchecked_fix_endianess<i32>(instream);
   auto sequences = Vec<sequence::Sequence>(sequence_count);
   for (auto i = 0; i < sequence_count; ++i) {
      sequences[i] = shape::io::read_sequence(instream);
   }

   //
   auto materials = shape::io::read_materials(instream, real_version);

   //
   auto node_count = io::read_unchecked<i32>(stream32);
   auto object_count = io::read_unchecked<i32>(stream32);
   auto decal_count = io::read_unchecked<i32>(stream32);
   auto sub_shape_count = io::read_unchecked<i32>(stream32);
   auto ifl_material_count = io::read_unchecked<i32>(stream32);
   auto node_rot_count = io::read_unchecked<i32>(stream32);
   auto node_trans_count = io::read_unchecked<i32>(stream32);
   auto node_uniform_scale_count = io::read_unchecked<i32>(stream32);
   auto node_aligned_scales_count = io::read_unchecked<i32>(stream32);
   auto node_arbitrary_scales_count = io::read_unchecked<i32>(stream32);
   // NOLINTBEGIN(readability-magic-numbers)
   auto ground_frame_count =
       (version > 23) ? io::read_unchecked<i32>(stream32) : 0;
   // NOLINTEND(readability-magic-numbers)
   auto object_state_count = io::read_unchecked<i32>(stream32);
   auto decal_state_count = io::read_unchecked<i32>(stream32);
   auto trigger_count = io::read_unchecked<i32>(stream32);
   auto detail_count = io::read_unchecked<i32>(stream32);
   auto meshes_count = io::read_unchecked<i32>(stream32);
   auto name_count = io::read_unchecked<i32>(stream32);
   auto smallest_visible_size = io::read_unchecked<i32>(stream32);
   auto smallest_visible_dl = io::read_unchecked<i32>(stream32);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto radius = io::read_unchecked<f32>(stream32);
   auto radius_tube = io::read_unchecked<f32>(stream32);
   auto center = io::read_unchecked<vec3>(stream32);
   auto bounds = io::read_unchecked<vec6>(stream32);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto nodes = io::read_unchecked<shape::Node>(stream32, node_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto objects = io::read_unchecked<shape::Object>(stream32, object_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   constexpr auto DECAL_SIZE = 5;
   io::seek_unchecked<i32>(stream32, decal_count * DECAL_SIZE);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   constexpr auto IFL_MATERIAL_SIZE = 5;
   io::seek_unchecked<i32>(stream32, ifl_material_count * IFL_MATERIAL_SIZE);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto sub_shape_first_nodes =
       io::read_unchecked<i32>(stream32, sub_shape_count);
   auto sub_shape_first_objects =
       io::read_unchecked<i32>(stream32, sub_shape_count);

   io::seek_unchecked<i32>(stream32, sub_shape_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto sub_shape_nodes = io::read_unchecked<i32>(stream32, sub_shape_count);
   auto sub_shape_objects = io::read_unchecked<i32>(stream32, sub_shape_count);

   io::seek_unchecked<i32>(stream32, sub_shape_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto default_rotations = io::read_unchecked<i16quat>(stream16, node_count);
   auto default_translations = io::read_unchecked<vec3>(stream32, node_count);
   auto node_translations =
       io::read_unchecked<vec3>(stream32, node_trans_count);
   auto node_rotations = io::read_unchecked<i16quat>(stream16, node_rot_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto node_uniform_scales =
       io::read_unchecked<f32>(stream32, node_uniform_scale_count);
   auto node_aligned_scales =
       io::read_unchecked<vec3>(stream32, node_aligned_scales_count);
   auto node_arbitrary_scale_factors =
       io::read_unchecked<vec3>(stream32, node_arbitrary_scales_count);
   auto node_arbitrary_scale_rotations =
       io::read_unchecked<i16quat>(stream32, node_arbitrary_scales_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto ground_frames = Vec<vec3>{};
   auto ground_rotations = Vec<i16quat>{};
   // NOLINTBEGIN(readability-magic-numbers)
   if (real_version > 23) {
      // NOLINTEND(readability-magic-numbers)
      ground_frames = io::read_unchecked<vec3>(stream32, ground_frame_count);
      ground_rotations =
          io::read_unchecked<i16quat>(stream16, ground_frame_count);

      io::guard_unchecked<i32>(stream32);
      io::guard_unchecked<i16>(stream16);
      io::guard_unchecked<i8>(stream8);
   }

   auto object_states =
       io::read_unchecked<shape::ObjectState>(stream32, object_state_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   io::seek_unchecked<i32>(stream32, decal_state_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto triggers = io::read_unchecked<shape::Trigger>(stream32, trigger_count);

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   Vec<shape::Detail> details;
   details.reserve(static_cast<usize>(detail_count));
   for (auto i = 0; i < detail_count; ++i) {
      details.push_back(shape::io::read_detail(stream32, real_version));
   }

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   Vec<mesh::MeshType> meshes;
   meshes.resize(meshes_count);
   for (auto i = 0; i < meshes_count; ++i) {
      meshes[i] =
          shape::io::read_mesh(stream32, stream16, stream8, real_version);
   }

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   auto names = std::views::istream<std::byte>(stream8) |
                std::views::transform([](auto byte) -> char8_t {
                   return static_cast<char8_t>(byte);
                }) |
                std::views::lazy_split(u8'\0') | std::views::take(name_count) |
                std::views::transform([](auto range) -> std::u8string {
                   return std::ranges::to<std::u8string>(range);
                }) |
                std::ranges::to<Vec<std::u8string>>();

   io::guard_unchecked<i32>(stream32);
   io::guard_unchecked<i16>(stream16);
   io::guard_unchecked<i8>(stream8);

   return std::expected<shape::Shape, std::string>(shape::Shape{
       .version = real_version,
       .materials = materials.value_or(material::Materials{}),
       .details = std::move(details),
       .triggers = std::move(triggers),
       .objects = std::move(objects),
       .object_states = std::move(object_states),
       .names = std::move(names),
       .nodes = std::move(nodes),
       .sequences = std::move(sequences),
       .meshes = std::move(meshes),
       .smallest_visible_size = smallest_visible_size,
       .smallest_visible_dl = smallest_visible_dl,
       .radius = radius,
       .radius_tube = radius_tube,
       .center = center,
       .bounds = bounds,
       .sub_shape_first_nodes = std::move(sub_shape_first_nodes),
       .sub_shape_first_objects = std::move(sub_shape_first_objects),
       .sub_shape_nodes = std::move(sub_shape_nodes),
       .sub_shape_objects = std::move(sub_shape_objects),
       .default_rotations = std::move(default_rotations),
       .default_translations = std::move(default_translations),
       .node_translations = std::move(node_translations),
       .node_rotations = std::move(node_rotations),
       .node_uniform_scales = std::move(node_uniform_scales),
       .node_aligned_scales = std::move(node_aligned_scales),
       .node_arbitrary_scale_factors = std::move(node_arbitrary_scale_factors),
       .node_arbitrary_scale_rotations =
           std::move(node_arbitrary_scale_rotations),
       .ground_translations = std::move(ground_frames),
       .ground_rotations = std::move(ground_rotations),
   });
}

} // namespace dts::shape
