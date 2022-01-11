#include <Galaxy/Physics/physics.hpp>
float sqr(float x) { return x*x; }
void SphereCollider::fill_params(Object *obj)
{
    radius = obj->scale.x;
    pos = obj->position;
}
bool SphereCollider::is_colliding(Collider *other)
{
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        if ((pos - sphere->pos).sqr_magnitude() <= sqr(radius+sphere->radius))
            return 1;
        return 0;
    }
    //else if (auto cube = dynamic_cast<CubeCollider*>(other))
    //{
    //    return 0;
    //}
    fprintf(stderr, "error: collision not implemented\n");
    return 0;
}