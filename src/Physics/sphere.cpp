#include <Galaxy/Physics/physics.hpp>
#include <stdio.h>

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
    //Vector3 intersection = ray.start + dist * ray.dir;
    // put the intersection into local space of sphere, then normalize (radius is magnitude)
    Vector3 normal = (intersection - _pos) / _radius;
    return RayResult(intersection, normal, dist);
    

    // source: https://gamedev.stackexchange.com/questions/20815/most-efficient-bounding-sphere-vs-ray-collision-algorithms?rq=1
    //Vector3 sphereToRay = other.start - _pos;
    //Vector3 dir = other.dir.unit();
    //if (sphereToRay.sqr_magnitude() < sqr(_radius))
    //    return RayResult(other.start, -dir, 0); // ray is inside
//
    //float proj = Vector3::dot(sphereToRay, dir);
    //if (proj >= 0)
    //    return RayResult(); // ray starts past sphere
    //
    //Vector3 projVec = dir*proj;
    //Vector3 closestToSphere = (sphereToRay - projVec);
    //if (closestToSphere.sqr_magnitude() < sqr(_radius))
    //{
    //    Vector3 hitPoint = _pos + closestToSphere - (dir * (_radius + closestToSphere.magnitude()));
    //    Vector3 normal = (hitPoint - _pos) / _radius;
    //    return RayResult(hitPoint, normal, (hitPoint - other.start).magnitude());
    //}
    //return RayResult();
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