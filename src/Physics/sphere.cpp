#include <Galaxy/Physics/physics.hpp>
#include "combinations.hpp"
#include <cmath>
static float sqr(float x) { return x*x; }
void SphereCollider::fill_params(Object *obj)
{
    radius = obj->scale.x/2;
    pos = obj->position;
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
            //return CollisionData{.penetration=sqrtf(sqrMag)-totalRadius, .dir=dir};
        return CollisionData();
    }
    else if (auto cube = dynamic_cast<CubeCollider*>(other))
    {
        //auto data = sphere_cube_collision(this, cube);
        //data.dir = -data.dir;
        //return data;
        return sphere_cube_collision(this, cube);
    }
    fprintf(stderr, "error: collision not implemented\n");
    return CollisionData();
}