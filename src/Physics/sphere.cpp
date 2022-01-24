#include <Galaxy/Physics/physics.hpp>
#include "combinations.hpp"
#include <cmath>
static float sqr(float x) { return x*x; }
void SphereCollider::fill_params(Object *obj)
{
    radius = obj->scale.x/2;
    pos = obj->position;
}
RayResult SphereCollider::is_colliding(const Ray &other)
{
    // source: https://gamedev.stackexchange.com/questions/20815/most-efficient-bounding-sphere-vs-ray-collision-algorithms?rq=1
    Vector3 sphereToRay = other.start - pos;
    if (sphereToRay.sqr_magnitude() < 1)
        return RayResult(); // ray is inside

    float proj = Vector3::dot(sphereToRay, other.dir);
    if (proj >= 0)
        return RayResult(); // ray starts past sphere
    
    return RayResult();
}
CollisionData SphereCollider::is_colliding(Collider *other)
{
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        Vector3 dir = (pos - sphere->pos);
        float sqrMag = dir.sqr_magnitude();
        float totalRadius = radius+sphere->radius;

        if (sqrMag <= sqr(totalRadius))
            return CollisionData(dir* (sqrtf(sqrMag)-totalRadius));
        return CollisionData();
    }
    else if (auto cube = dynamic_cast<CubeCollider*>(other))
    {
        return sphere_cube_collision(this, cube);
    }
    fprintf(stderr, "error: collision not implemented\n");
    return CollisionData();
}