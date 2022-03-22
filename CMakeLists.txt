message("Platform: '${CMAKE_SYSTEM_NAME}'")
if (CMAKE_SYSTEM_NAME MATCHES "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
    set(CMAKE_OSX_SYSROOT "iphoneos" CACHE STRING "")
    add_definitions(-arch arm64)
endif()
project("Galaxy Engine" LANGUAGES C CXX OBJC OBJCXX)
message("Platform: '${CMAKE_SYSTEM_NAME}'")
cmake_minimum_required(VERSION 3.19.7, FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

set(proj_dir ${CMAKE_CURRENT_LIST_DIR})
set(bin_dir ${CMAKE_CURRENT_BINARY_DIR})
set(src_dir ${proj_dir}/src)
file(RELATIVE_PATH relative_bin_dir "${proj_dir}/bin" "${bin_dir}")

include(initialize.cmake)

include_directories(${proj_dir}/src)
include_directories(${proj_dir}/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (RELEASE_BUILD)
    add_definitions(-O3)
else()
    add_definitions(-O3)
    add_definitions(-DDEBUG -DCMAKE_BUILD_TYPE=Debug -g)
endif()

add_definitions(-DGL_SILENCE_DEPRECATION -DGLES_SILENCE_DEPRECATION
    -DGLFW_INCLUDE_GLCOREARB -DOPENAL_DEPRECATED=)
add_definitions(-Wall -pedantic -Wextra)
add_definitions(-Wno-unused-parameter -Wno-unused-function)

# For testing
#add_definitions(-Weverything
#    -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-documentation
#    -Wno-newline-eof -Wno-zero-as-null-pointer-constant -Wno-documentation-unknown-command
#    -Wno-old-style-cast -Wno-shorten-64-to-32 -Wno-shadow-field-in-constructor
#    -Wno-sign-conversion -Wno-missing-prototypes -Wno-ctad-maybe-unsupported
#    -Wno-padded)
if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(EMSCRIPTEN 1)
endif()

if (EMSCRIPTEN)
    add_library(engine STATIC ${ENGINE_SRC})
    #set_source_files_properties(
    #    src/External/GLFM/glfm.h
    #    PROPERTIES COMPILE_FLAGS -Wno-nested-anon-types -Wno-gnu-zero-variadic-macro-arguments
    #)
    # For glfm
    add_definitions(-Wno-nested-anon-types -Wno-gnu-zero-variadic-macro-arguments)
endif()

if (APPLE)
    add_library(engine STATIC ${ENGINE_SRC})
    macro(add_fw fwname)
        target_link_libraries(engine "-framework ${fwname}")
    endmacro(add_fw)

    if (IOS)
        add_definitions(-fembed-bitcode)
        #set_source_files_properties(
        #    src/External/GLFM/glfm.h
        #    PROPERTIES COMPILE_FLAGS -Wno-nested-anon-types
        #)
        add_definitions(-Wno-nested-anon-types) # for GLFM
        add_definitions(-Wno-shorten-64-to-32) # for Xcode
    else()
        add_fw(Cocoa)
        add_fw(IOKit)
        add_fw(OpenGL)
        add_fw(OpenAL)
    endif()
endif()
target_precompile_headers(engine PRIVATE src/pch.h)
if (APPLE AND NOT IOS)
    target_link_libraries(engine ${proj_dir}/lib/libglfw3.a)
endif()
