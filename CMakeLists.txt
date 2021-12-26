include(initialize.cmake)

if (CMAKE_SYSTEM_NAME MATCHES "iOS")
#    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
    set(CMAKE_OSX_SYSROOT "iphoneos" CACHE STRING "")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "")
endif()

project("Galaxy Engine")
cmake_minimum_required(VERSION 3.19.7, FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

set(proj_dir ${CMAKE_CURRENT_LIST_DIR})
set(src_dir ${proj_dir}/src)

include_directories(${proj_dir}/src)
include_directories(${proj_dir}/include)

add_definitions(-DDEBUG)
add_definitions(-DCMAKE_BUILD_TYPE=Debug)
add_definitions(-g)

add_definitions(-DGL_SILENCE_DEPRECATION)
add_definitions(-DGLES_SILENCE_DEPRECATION)
add_definitions(-DGLFW_INCLUDE_GLCOREARB)

#set(ENGINE_ROOT "${proj_dir}")
#include("${proj_dir}/include/Galaxy/eheaders.cmake")

if (APPLE)
    #if (IOS)
    #    if (${CMAKE_OSX_SYSROOT} MATCHES "(MacOS)+")
    #        set(CMAKE_OSX_SYSROOT "iphoneos")
    #    endif()
    #endif()

    #add_library(engine STATIC ${ENGINE_SRC} ${ENGINE_HEADERS})
    add_library(engine STATIC ${ENGINE_SRC})
    macro(add_fw fwname)
        target_link_libraries(engine "-framework ${fwname}")
    endmacro(add_fw)

    if (IOS)
        add_definitions(-fembed-bitcode)
        #set_target_properties(engine PROPERTIES
        #    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Galaxy Engine.build/lib" # For Archiving
        #    XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos iphonesimulator"
        #    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 13.0
        #    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        #    XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES # Automatically import frameworks
        #)
        #set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "")
        #set(CMAKE_OSX_SYSROOT "iphoneos" CACHE STRING "")
        #set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "")
        #set(CMAKE_FRAMEWORK_PATH )
        #source_group(src FILES ${ENGINE_SRC})
        #source_group(include FILES ${ENGINE_HEADERS})
    else()
        add_fw(Cocoa)
        add_fw(IOKit)
        add_fw(OpenGL)
    endif()
endif()
if ((APPLE AND NOT IOS) OR WIN32)
    target_link_libraries(engine ${proj_dir}/lib/libglfw3.a)
endif()
