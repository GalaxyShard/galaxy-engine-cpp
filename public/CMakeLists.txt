# Copies resources on iOS
#add_custom_command(
#    TARGET ${NAME}
#    POST_BUILD
#    COMMAND /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -exclude CVS -exclude .svn -resolve-src-symlinks ${RES_DIR} ${APP_NAME}
#)
if (NOT DEFINED PLATFORM)
    set(PLATFORM "osx" CACHE STRING "")
endif()

if (PLATFORM MATCHES osx)
    set(MACOS 1)
elseif(PLATFORM MATCHES ios)
    set(CMAKE_SYSTEM_NAME "iOS")
    set(IOS 1)
elseif(PLATFORM MATCHES ios-vsc)
    # For vs code compile-commands.json
    set(CMAKE_SYSTEM_NAME "iOS")
    set(IOS 1)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
    set(CMAKE_OSX_SYSROOT "iphoneos" CACHE STRING "")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "")
    add_definitions(-arch arm64)
elseif(PLATFORM MATCHES web)
    set(CMAKE_SYSTEM_NAME "Emscripten")
    set(EMSCRIPTEN 1)
endif()
#if (IOS)
#    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
#    set(CMAKE_OSX_SYSROOT "iphoneos" CACHE STRING "")
#    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "")
#    add_definitions(-arch arm64)
#endif()

project("program" LANGUAGES C CXX OBJC OBJCXX)
cmake_minimum_required(VERSION 3.19.7, FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

set(proj_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(bin_dir ${CMAKE_CURRENT_BINARY_DIR})
set(src_dir ${proj_dir}/src)
file(RELATIVE_PATH relative_bin_dir "${proj_dir}/bin" "${bin_dir}")

include(initialize.cmake)

macro(checkvar var)
    if (NOT DEFINED "${var}")
        message(FATAL_ERROR "${var} not found")
    endif()
endmacro()
checkvar(OUT_NAME)
checkvar(GAME_SRC)
checkvar(BUNDLE_ID)

if (NOT DEFINED ENGINE_ROOT)
    set(ENGINE_ROOT "$ENV{HOME}/Projects/Galaxy-Engine")
endif()

include_directories(${ENGINE_ROOT}/include)
include_directories(${src_dir})
include_directories(${proj_dir}/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(RELEASE_BUILD)
    add_definitions(-O3)
else()
    if (PLATFORM MATCHES web)
        add_definitions(-O3) # WebGL doesnt support Og
    else()
        add_definitions(-Og)
    endif()
    set(CMAKE_BUILD_TYPE Debug)
    add_definitions(-DDEBUG -g)
endif()

#add_link_options(-fsanitize=undefined,address -fno-omit-frame-pointer)
#add_compile_options(-fsanitize=undefined,address -fno-omit-frame-pointer)

add_definitions(-DGL_SILENCE_DEPRECATION -DGLES_SILENCE_DEPRECATION
    -DGLFW_INCLUDE_GLCOREARB -DOPENAL_DEPRECATED=)
add_definitions(-Wall -pedantic -Wextra)
add_definitions(-Wno-unused-parameter -Wno-unused-function)

if (APPLE)
    # https://stackoverflow.com/questions/822404/how-to-set-up-cmake-to-build-an-app-for-the-iphone
    
    # The icon file name in the plist
    set(MACOSX_BUNDLE_ICON_FILE appIcon)
    set(CMAKE_MACOSX_BUNDLE 1)

    set(icon_path ${proj_dir}/assets/appIcon.icns)
    if (EXISTS ${icon_path})
        # Duplicates the appIcons.icns into "Resources"
        set_source_files_properties(
            ${icon_path} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
        )
    endif()
    add_executable(game ${GAME_SRC})
    target_precompile_headers(game PRIVATE ${ENGINE_ROOT}/include/Galaxy/engine.hpp)

    macro(add_fw fwname)
        target_link_libraries(game "-framework ${fwname}")
    endmacro(add_fw)

    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${BUNDLE_ID})

    if (IOS)
        add_definitions(-Wno-shorten-64-to-32) # for Xcode
        set_target_properties(game PROPERTIES
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${BUNDLE_ID}"
            XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos iphonesimulator"
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 11.0         # Version required for Galaxy Engine
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
            XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"            # iPhone, iPad
            XCODE_ATTRIBUTE_USE_HEADERMAP YES                       # Avoid header search paths
            XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES NO                 # For Archiving
            XCODE_ATTRIBUTE_OTHER_LDFLAGS ""                        # For Archiving
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"        # For Archiving
            XCODE_ATTRIBUTE_SKIP_INSTALL NO                         # For Archiving
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"   # For convenience
            XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IOS_DEV_ID}"
            XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES                # Auto adds frameworks
        )
        # GLFM
        add_fw(UIKit)
        add_fw(CoreHaptics)
        add_fw(CoreMotion)
        add_fw(CoreGraphics)
        add_fw(MetalKit)
        add_fw(OpenGLES)
        add_fw(Foundation)
        add_fw(Metal)
        add_fw(QuartzCore)

        # Galaxy Engine
        add_fw(OpenAL)

        set_source_files_properties(LaunchScreen.storyboard PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        
        # LaunchScreen needed to allow any screen size. Don't overwrite. (GLFM)
        if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/LaunchScreen.storyboard)
        file(READ "$ENV{HOME}/Projects/cmake/cmake_launchscreen.txt" launchscreen)
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/LaunchScreen.storyboard" "${launchscreen}")
        endif()
        # In place of MacOSXBundleInfo.plist.in (GLFM)
        file(READ "$ENV{HOME}/Projects/cmake/cmake_infoplist.txt" infoplist)
        string(REPLACE "MACOSX_BUNDLE_SHORT_VERSION_STRING" "1.0" infoplist "${infoplist}")
        string(REPLACE "MACOSX_BUNDLE_BUNDLE_VERSION" "1" infoplist "${infoplist}")
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMake-Info.plist.in" "${infoplist}")
        set_property(TARGET game PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/CMake-Info.plist.in)
        
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ENGINE_ROOT}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/galaxy_assets"
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${proj_dir}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/assets"
        )
        set_source_files_properties(
            ${CMAKE_CURRENT_BINARY_DIR}/assets PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
        )
        set_source_files_properties(
            ${CMAKE_CURRENT_BINARY_DIR}/galaxy_assets PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
        )
        target_sources(game
            PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/galaxy_assets
            PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/assets
        )
    else()
        # GLFW
        add_fw(Cocoa)
        add_fw(IOKit)
        add_fw(OpenGL)

        # Engine
        add_fw(OpenAL)
        
        # https://stackoverflow.com/questions/13429656/how-to-copy-contents-of-a-directory-into-build-directory-after-make-with-cmake
        add_custom_command(
            TARGET game
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${proj_dir}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/${OUT_NAME}.app/Contents/Resources/assets"
        )
        add_custom_command(
            TARGET game
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ENGINE_ROOT}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/${OUT_NAME}.app/Contents/Resources/galaxy_assets"
        )
    endif()
endif()
if (EMSCRIPTEN)

    # HACK: Make modifications to shell_minimal.html to take up the entire browser window
    file(READ ${EMSCRIPTEN_ROOT_PATH}/src/shell_minimal.html EMSCRIPTEN_SHELL_HTML)
    string(FIND "${EMSCRIPTEN_SHELL_HTML}" "<style>" HAS_STYLE)
    if (${HAS_STYLE} EQUAL -1)
        message(WARNING "<style> not found in shell_minimal.html, copying as-is")
    else()
        string(CONCAT STYLE_REPLACEMENT "<meta name=\"viewport\" content=\"width=device-width,user-scalable=no,viewport-fit=cover\">\n"
            "    <meta name=\"apple-mobile-web-app-capable\" content=\"yes\">\n"
            "    <style>\n"
            "      /* GLFM: Start changes */\n"
            "      :root {\n"
            "          --glfm-chrome-top-old: constant(safe-area-inset-top);\n"
            "          --glfm-chrome-right-old: constant(safe-area-inset-right);\n"
            "          --glfm-chrome-bottom-old: constant(safe-area-inset-bottom);\n"
            "          --glfm-chrome-left-old: constant(safe-area-inset-left);\n"
            "          --glfm-chrome-top: env(safe-area-inset-top);\n"
            "          --glfm-chrome-right: env(safe-area-inset-right);\n"
            "          --glfm-chrome-bottom: env(safe-area-inset-bottom);\n"
            "          --glfm-chrome-left: env(safe-area-inset-left);\n"
            "      }\n"
            "      body, html { border: 0px none; padding: 0px; margin: 0px; width: 100%; height: 100%; overflow: hidden; position: fixed; }\n"
            "      canvas.emscripten { background: black; width: 100%; height: 100%; }\n"
            "      .emscripten_border { width: 100%; height: 100%; border: 0px none !important;}\n"
            "      hr { display: none; }\n"
            "      /* GLFM: End changes */"
        )
        string(REPLACE "<style>" "${STYLE_REPLACEMENT}" EMSCRIPTEN_SHELL_HTML "${EMSCRIPTEN_SHELL_HTML}")
    endif()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/shell.html.in "${EMSCRIPTEN_SHELL_HTML}")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(game ${GAME_SRC})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ENGINE_ROOT}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/res/galaxy_assets"
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${proj_dir}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/res/assets"
    )
    set_target_properties(game PROPERTIES LINK_FLAGS "--shell-file \"${CMAKE_CURRENT_BINARY_DIR}/shell.html.in\" --preload-file \"${proj_dir}/bin/web/res@bin/web/res\"")
endif()
set_target_properties(game PROPERTIES OUTPUT_NAME "${OUT_NAME}")

# Galaxy Engine
if (MACOS)
    target_link_libraries(game "${ENGINE_ROOT}/lib/libglfw3.a")
endif()

target_link_libraries(game "${ENGINE_ROOT}/bin/${relative_bin_dir}/libengine.a")