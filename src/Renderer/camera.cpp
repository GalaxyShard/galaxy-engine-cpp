#include <Galaxy/Renderer/camera.hpp>

//Camera *Camera::main = new Camera();
auto Camera::main = std::make_unique<Camera>();
