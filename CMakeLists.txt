# OSX
# cmake -B bin/osx
# make -C bin/osx -j8

# IOS
# cmake -B bin/ios -DIOS=1
# make -C bin/ios -j8

# WebGL
# emcmake cmake -B bin/web -DRELEASE=1
# emmake make -C bin/web -j8

if (IOS) # Set on command line -DIOS=1
    set(CMAKE_SYSTEM_NAME "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
    set(CMAKE_OSX_SYSROOT "iphoneos" CACHE STRING "")
    add_definitions(-arch arm64)
endif()
project("Galaxy Engine" LANGUAGES C CXX OBJC OBJCXX)
cmake_minimum_required(VERSION 3.19.7, FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

if (APPLE AND NOT IOS)
    set(MACOS 1)
endif()

set(proj_dir ${CMAKE_CURRENT_LIST_DIR})
set(bin_dir ${CMAKE_CURRENT_BINARY_DIR})
set(src_dir ${proj_dir}/src)
file(RELATIVE_PATH relative_bin_dir "${proj_dir}/bin" "${bin_dir}")

include(initialize.cmake)

include_directories(${proj_dir}/src)
include_directories(${proj_dir}/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (RELEASE) # -DRELEASE=1
    add_definitions(-O3)
else()
    if (EMSCRIPTEN)
        add_definitions(-O3) # WebGL doesnt support Og
    else()
        add_definitions(-Og)
    endif()
    set(CMAKE_BUILD_TYPE Debug)
    add_definitions(-DDEBUG -g)
endif()

# Remove comment to enable clang UBsan & Asan, works on WebGL as well
#add_link_options(-fsanitize=undefined,address -fno-omit-frame-pointer)
#add_compile_options(-fsanitize=undefined,address -fno-omit-frame-pointer)


add_definitions(-DGL_SILENCE_DEPRECATION -DGLES_SILENCE_DEPRECATION
    -DGLFW_INCLUDE_GLCOREARB -DOPENAL_DEPRECATED=)
add_definitions(-Wall -pedantic -Wextra)
add_definitions(-Wno-unused-parameter -Wno-unused-function)

add_library(engine STATIC ${ENGINE_SRC})
if (EMSCRIPTEN)
    # For glfm
    add_definitions(-Wno-nested-anon-types -Wno-gnu-zero-variadic-macro-arguments)
endif()

if (APPLE)
    macro(add_fw fwname)
        target_link_libraries(engine "-framework ${fwname}")
    endmacro(add_fw)

    if (IOS)
        add_definitions(-Wno-nested-anon-types) # for GLFM
        add_definitions(-fembed-bitcode -Wno-shorten-64-to-32) # for Xcode
    else()
        add_fw(Cocoa)
        add_fw(IOKit)
        add_fw(OpenGL)
        add_fw(OpenAL)
    endif()
endif()
target_precompile_headers(engine PRIVATE src/pch.h)
if (MACOS)
    target_link_libraries(engine ${proj_dir}/lib/libglfw3.a)
endif()
