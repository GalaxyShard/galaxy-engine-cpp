#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
static float sqr(float x) { return x*x; }

//CollisionData::CollisionData(Vector3 dir, float penetration)
//    : dir(dir), penetration(penetration), isColliding(1) { }
CollisionData::CollisionData(Vector3 mtv) : dir(mtv), isColliding(1) { }
CollisionData::CollisionData() : dir(), isColliding(0) { }

std::vector<Vector3> get_points(CubeCollider *cube)
{
    auto p = std::vector<Vector3>();
    Matrix3x3 rot = Matrix3x3::rotate(cube->rotation);
    Vector3 corner = cube->scale*0.5f;
    p.push_back(Vector3(-1,-1,1));
    p.push_back(Vector3(1,-1,1));
    p.push_back(Vector3(-1,1,1));
    p.push_back(Vector3(1,1,1));

    p.push_back(Vector3(-1,-1,-1));
    p.push_back(Vector3(1,-1,-1));
    p.push_back(Vector3(-1,1,-1));
    p.push_back(Vector3(1,1,-1));
    for (int i = 0; i < 8; ++i)
        p[i] = rot*(corner*p[i])+cube->pos;

    //p.push_back(rot*(corner*Vector3(-1,-1,1))+cube->pos);
    //p.push_back(rot*(corner*Vector3(1,-1,1))+cube->pos);
    //p.push_back(rot*(corner*Vector3(-1,1,1))+cube->pos);
    //p.push_back(rot*(corner*Vector3(1,1,1))+cube->pos);

    //p.push_back(rot*(corner*Vector3(-1,-1,-1))+cube->pos);
    //p.push_back(rot*(corner*Vector3(1,-1,-1))+cube->pos);
    //p.push_back(rot*(corner*Vector3(-1,1,-1))+cube->pos);
    //p.push_back(rot*(corner*Vector3(1,1,-1))+cube->pos);
    return p;
};
CollisionData sphere_cube_collision(SphereCollider *sphere, CubeCollider *cube)
{
    auto points = get_points(cube);
    Vector3 sphereToCube = cube->pos - sphere->pos;
    float mag = sphereToCube.magnitude();
    Vector3 axis = sphereToCube/mag;

    float maxProj = Vector3::dot(points[0]-cube->pos, axis);
    for (int i = 1; i < 8; ++i)
    {
        float proj = Vector3::dot(points[i]-cube->pos, axis);
        if (maxProj < proj) maxProj = proj;
    }
    // todo: fix this, not 100% correct
    float penetration = -(mag - sphere->radius - maxProj);
    if (penetration > 0)
        return CollisionData(axis*penetration);
        //return CollisionData(axis, penetration);
        //return CollisionData{.penetration=penetration, .dir=axis};
    return CollisionData();
}