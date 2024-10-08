#include <Galaxy/Physics/physics.hpp>
#include <stdio.h>
#include <Galaxy/print.hpp>

#include "combinations.hpp"
#include <cmath>
static float sqr(float x) { return x*x; }
float SphereCollider::radius() { return _radius; }
RayResult SphereCollider::is_colliding(const Ray &ray)
{
    Vector3 sphereToRay = ray.start - _pos;
    float b = Vector3::dot(sphereToRay, ray.dir);
    float c = sphereToRay.sqr_magnitude() - sqr(_radius);

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
    if (c > 0.0f && b > 0.0f) return RayResult();
    float discr = sqr(b) - c;

    // A negative discriminant corresponds to ray missing sphere 
    if (discr < 0.0f) return RayResult();

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float dist = -b - sqrtf(discr);

    // If t is negative, ray started inside sphere so clamp t to zero 
    if (dist < 0.0f) dist = 0.0f;
    Vector3 intersection = ray.dir * dist + ray.start;
    // put the intersection into local space of sphere, then normalize (radius is magnitude)
    Vector3 normal = (intersection - _pos) / _radius;
    return RayResult(intersection, normal, dist);
}
CollisionData SphereCollider::is_colliding(Collider *other)
{
    if (refreshAABB)
        fix_bounding_box();
    if (other->needs_refresh())
        other->fix_bounding_box();
    if (!aabb_test(other))
        return CollisionData();

    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        Vector3 dir = (_pos - sphere->_pos);
        float sqrMag = dir.sqr_magnitude();
        float totalRadius = _radius + sphere->_radius;

        if (sqrMag <= sqr(totalRadius))
            return CollisionData(dir * (totalRadius-sqrtf(sqrMag)));
        return CollisionData();
    }
    else if (auto cube = dynamic_cast<CubeCollider*>(other))
    {
        return sphere_cube_collision(this, cube);
    }
    logerr("error: collision not implemented\n");
    return CollisionData();
}
void SphereCollider::fix_bounding_box()
{
    _pos = obj->position;
    _radius = obj->scale.x*0.5f;
    aabbMin = _pos - _radius;
    aabbMax = _pos + _radius;
    refreshAABB = 0;
}
void SphereCollider::refresh()
{
    refreshAABB = 1;
}