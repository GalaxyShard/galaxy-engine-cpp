const std = @import("std");

const find_build_sources = @import("find-build-sources.zig");

fn isCppFile(path: []const u8) bool {
    return std.mem.endsWith(u8, path, ".cpp");
}

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const engine = b.addStaticLibrary(.{
        .name = "galaxy-engine-cpp",
        .target = target,
        .optimize = optimize,
    });
    engine.linkLibCpp();


    const glfw = b.dependency("glfw", .{
        .target = target,
        .optimize = optimize,
    });

    const glfm = b.addStaticLibrary(.{
        .name = "glfm",
        .target = target,
        .optimize = optimize,
    });
    glfm.linkLibC();
    const glfm_file: ?[]const u8 = (
        if (target.result.os.tag == .ios)
            "vendored/GLFM/glfm_platform_ios.m"
        else if (target.result.os.tag == .emscripten)
            "vendored/GLFM/glfm_platform_emscripten.c"
        else if (target.result.abi == .android)
            "vendored/GLFM/glfm_platform_android.c"
        else
            null
    );
    if (glfm_file) |file| {
        glfm.addCSourceFile(.{
            .file = b.path(file),
        });
        engine.linkLibrary(glfm);
    } else {
        engine.linkLibrary(glfw.artifact("glfw"));
    }
    engine.linkSystemLibrary("OpenAL");


    const cpp_sources = try find_build_sources.find(b, "src", isCppFile);
    engine.addCSourceFiles(.{
        .root = cpp_sources.directory,
        .files = cpp_sources.inner,
        .flags = &.{ "-std=c++26", "-Wall", "-Wextra", "-Wpedantic" },
    });
    engine.addCSourceFile(.{
        .file = b.path("vendored/stb_image_init.cpp"),
    });

    if (target.result.os.tag == .ios) {
        engine.addCSourceFile(.{
            .file = b.path("vendored/GLFM/glfm_platform_ios.m"),
        });
    }
    if (target.result.os.tag == .emscripten) {
        engine.addCSourceFile(.{
            .file = b.path("vendored/GLFM/glfm_platform_emscripten.c"),
        });
    }
    engine.addIncludePath(glfw.artifact("glfw").getEmittedIncludeTree());
    engine.addIncludePath(b.path("src"));
    engine.addIncludePath(b.path("include"));
    engine.addIncludePath(b.path("vendored"));

    engine.installHeadersDirectory(b.path("include/Galaxy"), "Galaxy", .{
        .include_extensions = &.{ ".hpp", ".inl" },
    });

    b.installArtifact(engine);
}
