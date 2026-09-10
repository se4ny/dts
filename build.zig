const std = @import("std");
const zcc = @import("compile-commands");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const mod = b.addModule("dts", .{
        .target = target,
        .optimize = optimize,
        .link_libcpp = target.result.abi != .msvc,
        .link_libc = true,
    });
    mod.addCMacro("_DTS_BUILD", "1");
    mod.addIncludePath(b.path("include"));
    mod.addIncludePath(b.path("src"));
    mod.addCSourceFile(.{
        .file = b.path("src/shape/shape.cpp"),
        .flags = &.{"-std=c++26"},
    });

    const lib = b.addLibrary(.{
        .name = "dts",
        .root_module = mod,
    });
    lib.installHeadersDirectory(b.path("include"), "", .{
        .include_extensions = &.{ ".hpp", ".h" },
    });

    b.installArtifact(lib);

    // Tests
    const test_mod = b.addModule("tests", .{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    test_mod.addCSourceFile(.{
        .file = b.path("tests/main.cpp"),
        .flags = &.{
            "-std=c++26",
        },
    });
    test_mod.addIncludePath(b.path("include"));
    test_mod.linkLibrary(lib);

    const test_exe = b.addExecutable(.{
        .name = "tests",
        .root_module = test_mod,
    });
    const test_install = b.addInstallArtifact(test_exe, .{});

    const test_step = b.step("test", "Build test suite");
    test_step.dependOn(&test_install.step);
    // test_step.dependOn(&lib.step);

    // Compile commands
    // make a list of targets that have include files and c source files
    var targets: std.ArrayList(*std.Build.Step.Compile) = .empty;

    try targets.append(b.allocator, lib);
    try targets.append(b.allocator, test_exe);

    _ = zcc.createStep(b, .{
        .name = "cdb",
        .targets = targets.items,
    });
}
