#include <Galaxy/Renderer/camera.hpp>
#include <Galaxy/Renderer/renderer.hpp>

auto Camera::main = new Camera();

void Camera::override_projection()
{
    manualProjection = 1;
}
void Camera::refresh()
{
    if (manualProjection)
        return;
    if (mode == PERSPECTIVE)
    {
        projection = Matrix4x4::perspective(
            fov, ((float)Renderer::screenWidth)/Renderer::screenHeight,
            nearClip, farClip, 0
        );
    }
    else
    {
        const float s = orthoSize;
        const Vector2 &ratio = Renderer::aspectRatio;
        projection = Matrix4x4::ortho(-s*ratio.x, s*ratio.x, -s*ratio.y, s*ratio.y, nearClip, farClip, 0);
    }
}
void Camera::defaults()
{
    manualProjection = 0;
    position = Vector3(0,0,-5);
    rotation = Vector3();
    orthoSize = 5;
    nearClip = 0.1;
    farClip = 10000;
    fov = 60*Math::to_rad;
    mode = ORTHO;
}
void Camera::reset()
{
    defaults();
    set_bg(0, 0, 0);
    refresh();
}
void Camera::set_bg(float r, float g, float b)
{
    Renderer::set_clear_color(r, g, b, 1);
}