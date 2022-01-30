#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
static float sqr(float x) { return x*x; }
bool Physics::autoSimulate = 1;

RayResult::RayResult(Vector3 pos, Vector3 normal, float dist)
    : pos(pos), normal(normal), dist(dist), collided(1) { }
Ray::Ray(Vector3 start, Vector3 dir) : start(start), dir(dir) { }
CollisionData::CollisionData(Vector3 mtv) : dir(mtv), isColliding(1) { }
CollisionData::CollisionData() : dir(), isColliding(0) { }

void get_local_points(std::vector<Vector3> &p, CubeCollider *cube)
{
    Matrix3x3 rot = Matrix3x3::rotate(cube->obj->rotation);
    Vector3 corner = cube->obj->scale*0.5f;
    p.push_back(Vector3(-1,-1,1));
    p.push_back(Vector3(1,-1,1));
    p.push_back(Vector3(-1,1,1));
    p.push_back(Vector3(1,1,1));

    p.push_back(Vector3(-1,-1,-1));
    p.push_back(Vector3(1,-1,-1));
    p.push_back(Vector3(-1,1,-1));
    p.push_back(Vector3(1,1,-1));
    for (int i = 0; i < 8; ++i)
        p[i] = rot*(corner*p[i]);
};
void get_points(std::vector<Vector3> &p, CubeCollider *cube)
{
    get_local_points(p, cube);
    for (int i = 0; i < 8; ++i)
        p[i] += cube->obj->position;
}
CollisionData sphere_cube_collision(SphereCollider *sphere, CubeCollider *cube)
{
    Vector3 localSpherePos = Matrix3x3::rotate(cube->obj->rotation).transpose()*((sphere->obj->position - cube->obj->position));
    Vector3 cubeRadius = cube->obj->scale * 0.5f;
    Vector3 closest;
    for (int i = 0; i < 3; ++i)
        closest[i] = Math::clamp(localSpherePos[i], -cubeRadius[i], cubeRadius[i]);

    float sphereRadius = sphere->obj->scale.x*0.5f;
    Vector3 offset = localSpherePos - closest;
    if (offset.sqr_magnitude() < sqr(sphereRadius))
    {
        Vector3 mtv = offset.unit()*(sphereRadius - offset.magnitude());
        return CollisionData(Matrix3x3::rotate(cube->obj->rotation)*mtv);
    }
    return CollisionData();
}