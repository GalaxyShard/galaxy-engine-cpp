
set(ENGINE_SRC
    src/UI/font.cpp
    src/UI/group.cpp
    src/UI/image.cpp
    src/UI/text.cpp
    src/UI/uiobject.cpp

    src/Renderer/vertexbuffer.cpp
    src/Renderer/mesh.cpp
    src/Renderer/vertexlayout.cpp
    src/Renderer/camera.cpp
    src/Renderer/vertexarray.cpp
    src/Renderer/texture.cpp
    src/Renderer/shader.cpp
    src/Renderer/material.cpp
    src/Renderer/indexbuffer.cpp
    src/Renderer/renderer.cpp

    src/Math/vector4.cpp
    src/Math/vector3.cpp
    src/Math/matrix3x3.cpp
    src/Math/matrix4x4.cpp
    src/Math/vector2.cpp
    src/Math/math.cpp
    src/Math/quaternion.cpp
    src/Math/time.cpp

    src/Networking/client.cpp
    src/Networking/server.cpp
    src/Networking/shared.cpp

    src/Physics/combinations.cpp
    src/Physics/cube.cpp
    src/Physics/sphere.cpp

    src/Scene/scene_parser.cpp
    src/Scene/scene.cpp

    src/External/stb_image_init.cpp

    src/Input/input.cpp
    src/Assets/asset.cpp

    src/object.cpp
    src/engine.cpp
    src/init.cpp
    src/event.cpp
    src/main.cpp
)
macro(add_src files)
    set(ENGINE_SRC ${ENGINE_SRC} ${files})
endmacro()

if (APPLE)
    add_src(src/OS/Apple/file_handler.mm)
endif()
if (CMAKE_SYSTEM_NAME MATCHES "iOS")
    add_src(src/External/GLFM/glfm_platform_ios.m)

    add_src(src/OS/IOS/safe_area.mm)
endif()
if (CMAKE_SYSTEM_NAME MATCHES "Android")
    add_src(src/External/GLFM/glfm_platform_android.c)
endif()
if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    add_src(src/External/GLFM/glfm_platform_emscripten.c)
endif()