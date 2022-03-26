#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/Math/matrix.hpp>

class Camera
{
private:
public:
    static Camera *main;
    
    Vector3 position;
    Vector3 rotation;

    Matrix4x4 projection = Matrix4x4::identity();
    bool isPerspective = 0;
};