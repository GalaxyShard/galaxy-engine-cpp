#include <Galaxy/Physics/physics.hpp>

void CubeCollider::fill_params(Object *obj)
{

}
bool CubeCollider::is_colliding(Collider *other)
{
    if (auto cube = dynamic_cast<CubeCollider*>(other))
    {
        Vector3 centerDist = pos - cube->pos;
        Vector3 thisClosest = scale;
        Vector3 otherClosest = cube->scale*Vector3(-1,1,1);

        Vector3 axis = Vector3(1, -1, 0);
        axis = axis/axis.magnitude();

        float centerProj = Vector3::dot(centerDist, axis);
        float thisProj = Vector3::dot(thisClosest, axis);
        float otherProj = Vector3::dot(otherClosest, axis);
        float gap = centerProj - thisProj + otherProj;
        return gap <= 0;
    }
    fprintf(stderr, "error: collision not implemented\n");
    return 0;
}