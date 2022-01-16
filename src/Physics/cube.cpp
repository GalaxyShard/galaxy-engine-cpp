#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
bool within(Vector3 a, Vector3 min, Vector3 max)
{
    return Math::within(a.x, min.x, max.x)
        && Math::within(a.y, min.y, max.y)
        && Math::within(a.z, min.z, max.z);
}
// inefficient, sometimes fails at specific angles
// todo: use seperating axis test
bool cube0_collides_with(CubeCollider *cube0, CubeCollider *cube1)
{
    Matrix4x4 inverseRot = Matrix4x4::translate(cube0->pos)
        * Matrix4x4::rotate(cube0->rotation).transpose()
        * Matrix4x4::translate(-cube0->pos);
    Matrix3x3 rot = Matrix3x3::rotate(cube1->rotation);
    //Matrix4x4 inverseRot = Matrix4x4::identity();

    // rotate second around the first cube so that one cube is axis-aligned, then bounds check each point
    
    Vector3 thisRadius = cube0->scale*0.5f;
    Vector3 radius = (cube1->scale*0.5f);

    Vector3 min = cube0->pos - thisRadius;
    Vector3 max = cube0->pos + thisRadius;

    auto process = [&](float x, float y, float z)
    { return (Vector3)(inverseRot*(cube1->pos + rot*(radius*Vector3(x,y,z)))); };

    Vector3 leftDownClose = process(-1,-1,1);
    Vector3 rightDownClose = process(1,-1,1);
    Vector3 leftUpClose = process(-1,1,1);
    Vector3 rightUpClose = process(1,1,1);

    Vector3 leftDownFar = process(-1,-1,-1);
    Vector3 rightDownFar = process(1,-1,-1);
    Vector3 leftUpFar = process(-1,1,-1);
    Vector3 rightUpFar = process(1,1,-1);
    //Vector3 leftDownClose = inverseRot*(cube1->pos + rot*(radius*Vector3(-1,-1,1)));
    //Vector3 rightDownClose = inverseRot*(cube1->pos + rot*(radius*Vector3(1,-1,1)));
    //Vector3 leftUpClose = inverseRot*(cube1->pos + rot*(radius*Vector3(-1,1,1)));
    //Vector3 rightUpClose = inverseRot*(cube1->pos + rot*(radius*Vector3(1,1,1)));

    //Vector3 leftDownFar = inverseRot*(cube1->pos + rot*(radius*Vector3(-1,-1,-1)));
    //Vector3 rightDownFar = inverseRot*(cube1->pos + rot*(radius*Vector3(1,-1,-1)));
    //Vector3 leftUpFar = inverseRot*(cube1->pos + rot*(radius*Vector3(-1,1,-1)));
    //Vector3 rightUpFar = inverseRot*(cube1->pos + rot*(radius*Vector3(1,1,-1)));
    
    if (within(leftDownClose, min, max)
        || within(rightDownClose, min, max)
        || within(leftUpClose, min, max)
        || within(rightUpClose, min, max)

        || within(leftDownFar, min, max)
        || within(rightDownFar, min, max)
        || within(leftUpFar, min, max)
        || within(rightUpFar, min, max))
    {
        return 1;
    }
    return 0;
}

void CubeCollider::fill_params(Object *obj)
{
    pos = obj->position;
    rotation = obj->rotation;
    scale = obj->scale;
}
bool CubeCollider::is_colliding(Collider *other)
{
    if (auto cube = dynamic_cast<CubeCollider*>(other))
    {
        return cube0_collides_with(this, cube) || cube0_collides_with(cube, this);

        //Vector3 centerDist = pos - cube->pos;
        //Vector3 thisClosest = scale/2;
        //Vector3 otherClosest = cube->scale/2*Vector3(-1,1,1);

        //Vector3 axis = Vector3(1, -1, 0);
        //axis = axis/axis.magnitude();

        //float centerProj = Vector3::dot(centerDist, axis);
        //float thisProj = Vector3::dot(thisClosest, axis);
        //float otherProj = Vector3::dot(otherClosest, axis);
        //float gap = centerProj - thisProj + otherProj;
        //return gap <= 0;
    }
    fprintf(stderr, "error: collision not implemented\n");
    return 0;
}