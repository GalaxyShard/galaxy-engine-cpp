#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/Math/matrix.hpp>

#include <memory>
class Camera
{
private:
public:
    //static Camera *main;
    static std::unique_ptr<Camera> main;
    
    Vector3 position;
    Vector3 rotation;
    //Quaternion rotation = Quaternion::identity();

    Matrix4x4 projection = Matrix4x4::identity();
    bool isPerspective = 0;

    //enum Projection { Orthographic, Perspective };
    //Projection proj;

    //union
    //{
    //    struct
    //    {
            
    //    } ortho;

    //    struct
    //    {
    //        float fov;
    //    } perspective;
    //} properties;

    //float near, far;
};