#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/Math/matrix.hpp>

class Camera
{
public:
    enum CameraMode : ucharG { ORTHO, PERSPECTIVE };
    static Camera *main;
    
    Vector3 position;
    Vector3 rotation;

    Matrix4x4 projection;
    float orthoSize;
    float nearClip;
    float farClip;
    float fov;
    CameraMode mode = ORTHO;
private:
    bool manualProjection;
public:

    Camera() : projection(Matrix4x4::identity()) { defaults(); }

    void override_projection();
    void defaults();
    void refresh();
    void reset();
};