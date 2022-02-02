#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
static float sqr(float x) { return x*x; }
//bool Physics::autoSimulate = 1;

RayResult::RayResult(Vector3 pos, Vector3 normal, float dist)
    : pos(pos), normal(normal), dist(dist), collided(1) { }
Ray::Ray(Vector3 start, Vector3 dir) : start(start), dir(dir) { }
CollisionData::CollisionData(Vector3 mtv) : dir(mtv), isColliding(1) { }
CollisionData::CollisionData() : dir(), isColliding(0) { }

Vector3 Collider::pos() { return _pos; }
//void Collider::pos(Vector3 pos) { refreshAABB=1; _pos=pos; }
bool Collider::aabb_test(Collider *other)
{
    //if ((other->aabbMin.x >= aabbMax.x) || (aabbMin.x >= other->aabbMax.x))
    //    return 0;
    //if ((other->aabbMin.y >= aabbMax.y) || (aabbMin.y >= other->aabbMax.y))
    //    return 0;
    //return 1;

    // Equivalent to above (todo: test)
    return !(((other->aabbMin.x >= aabbMax.x) || (aabbMin.x >= other->aabbMax.x))
        && ((other->aabbMin.y >= aabbMax.y) || (aabbMin.y >= other->aabbMax.y)));
}
void get_local_points(std::vector<Vector3> &p, CubeCollider *cube)
{
    Matrix3x3 rot = Matrix3x3::rotate(cube->rotation());
    Vector3 corner = cube->scale()*0.5f;
//    Matrix3x3 rot = Matrix3x3::rotate(cube->obj->rotation);
//    Vector3 corner = cube->obj->scale*0.5f;
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
        p[i] += cube->pos();
        //p[i] += cube->obj->position;
}
CollisionData sphere_cube_collision(SphereCollider *sphere, CubeCollider *cube)
{
    //Vector3 localSpherePos = Matrix3x3::rotate(cube->obj->rotation).transpose()*((sphere->obj->position - cube->obj->position));
    //Vector3 cubeRadius = cube->obj->scale * 0.5f;
    Vector3 localSpherePos = Matrix3x3::rotate(cube->rotation()).transpose()*((sphere->pos() - cube->pos()));
    Vector3 cubeRadius = cube->scale() * 0.5f;
    Vector3 closest;
    for (int i = 0; i < 3; ++i)
        closest[i] = Math::clamp(localSpherePos[i], -cubeRadius[i], cubeRadius[i]);

    //float sphereRadius = sphere->obj->scale.x*0.5f;
    //float sphereRadius = sphere->obj->scale.x*0.5f;
    Vector3 offset = localSpherePos - closest;
    if (offset.sqr_magnitude() < sqr(sphere->radius()))
    {
        Vector3 mtv = offset.unit()*(sphere->radius() - offset.magnitude());
        //return CollisionData(Matrix3x3::rotate(cube->obj->rotation)*mtv);
        return CollisionData(Matrix3x3::rotate(cube->rotation())*mtv);
    }
    return CollisionData();
}