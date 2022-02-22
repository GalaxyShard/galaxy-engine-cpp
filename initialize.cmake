
set(ENGINE_SRC
    src/Assets/asset.cpp
    src/Audio/audio.cpp
    src/ECS/ecs.cpp
    src/External/stb_image_init.cpp
    src/Input/input.cpp

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
    src/Physics/rigidbody.cpp
    src/Physics/sphere.cpp

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

    src/Scene/scene_parser.cpp
    src/Scene/scene.cpp

    src/UI/font.cpp
    src/UI/group.cpp
    src/UI/image.cpp
    src/UI/text.cpp
    src/UI/uiobject.cpp

    src/callback.cpp
    src/object.cpp
    src/engine.cpp
    src/init.cpp
    src/event.cpp
    src/main.cpp
)
set(ENGINE_H
    PRIVATE
    include/Galaxy/engine.hpp
)
#set(ENGINE_H
#    PUBLIC
#    include/Galaxy/Assets/assets.hpp
#    include/Galaxy/Audio/audio.hpp
#    include/Galaxy/ECS/ecs.hpp
#    include/Galaxy/Events/callback.hpp
#    include/Galaxy/Events/event.hpp
#    include/Galaxy/Input/input.hpp
#    include/Galaxy/Input/keycodes.hpp
#    include/Galaxy/Math/gmath.hpp
#    include/Galaxy/Math/matrix.hpp
#    include/Galaxy/Math/time.hpp
#    include/Galaxy/Math/vector.hpp
#    include/Galaxy/Networking/networking.hpp
#    include/Galaxy/OS/defines.hpp
#    include/Galaxy/Physics/physics.hpp
#    include/Galaxy/Renderer/camera.hpp
#    include/Galaxy/Renderer/material.hpp
#    include/Galaxy/Renderer/mesh.hpp
#    include/Galaxy/Renderer/renderer.hpp
#    include/Galaxy/Renderer/shader.hpp
#    include/Galaxy/Renderer/texture.hpp
#    include/Galaxy/Scene/scene.hpp
#    include/Galaxy/UI/font.hpp
#    include/Galaxy/UI/group.hpp
#    include/Galaxy/UI/image.hpp
#    include/Galaxy/UI/text.hpp
#    include/Galaxy/init.hpp
#    include/Galaxy/object.hpp
#    include/Galaxy/print.hpp
#    include/Galaxy/types.hpp
#    include/Galaxy/engine.hpp
#)
macro(add_src files)
    set(ENGINE_SRC ${ENGINE_SRC} ${files})
endmacro()
macro(add_h files)
    set(ENGINE_H ${ENGINE_H} ${files})
endmacro()

if (APPLE)
    #add_src(src/External/metal_init.cpp)
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