#include <Galaxy/Physics/physics.hpp>
#include "combinations.hpp"
#include <cmath>
static float sqr(float x) { return x*x; }
float SphereCollider::radius() { return _radius; }
RayResult SphereCollider::is_colliding(const Ray &other)
{
    // source: https://gamedev.stackexchange.com/questions/20815/most-efficient-bounding-sphere-vs-ray-collision-algorithms?rq=1
    Vector3 sphereToRay = other.start - _pos;
    Vector3 dir = other.dir.unit();
    if (sphereToRay.sqr_magnitude() < sqr(_radius))
        return RayResult(other.start, -dir, 0); // ray is inside

    float proj = Vector3::dot(sphereToRay, dir);
    if (proj >= 0)
        return RayResult(); // ray starts past sphere
    
    Vector3 projVec = dir*proj;
    Vector3 closestToSphere = (sphereToRay - projVec);
    if (closestToSphere.sqr_magnitude() < sqr(_radius))
    {
        Vector3 hitPoint = _pos + closestToSphere - (dir * (_radius + closestToSphere.magnitude()));
        Vector3 normal = (hitPoint - _pos) / _radius;
        return RayResult(hitPoint, normal, (hitPoint - other.start).magnitude());
    }
    return RayResult();
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