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
    Vector3 dir = other.dir.unit();
    if (sphereToRay.sqr_magnitude() < sqr(radius))
        return RayResult(other.start, -dir, 0); // ray is inside

    float proj = Vector3::dot(sphereToRay, dir);
    if (proj >= 0)
        return RayResult(); // ray starts past sphere
    
    Vector3 projVec = dir*proj;
    Vector3 closestToSphere = (sphereToRay - projVec);
    if (closestToSphere.sqr_magnitude() < sqr(radius))
    {
        //shortestOnRay.magnitude();
        Vector3 hitPoint = pos + closestToSphere - (dir * (radius + closestToSphere.magnitude()));
        Vector3 normal = (hitPoint - pos) / radius;
        return RayResult(hitPoint, normal, (hitPoint - other.start).magnitude());
    }
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