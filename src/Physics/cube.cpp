#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
#include "combinations.hpp"
#include <cmath>
//CubeCollider::CubeCollider()
//{
//    refresh();
//}
Vector3 CubeCollider::scale() { return _scale; }
Vector3 CubeCollider::rotation() { return _rotation; }
//void CubeCollider::scale(Vector3 scale) { refreshAABB = 1; _scale=scale; }
//void CubeCollider::rotation(Vector3 rotation) { refreshAABB = 1; _rotation=rotation; }
RayResult CubeCollider::is_colliding(const Ray &other)
{
    // Axis-align box by rotating the ray around it
    //Matrix4x4 inverseRot = Matrix4x4::translate(obj->position) * Matrix4x4::rotate(obj->rotation).transpose() * Matrix4x4::translate(-obj->position);
    Matrix4x4 inverseRot = Matrix4x4::translate(_pos) * Matrix4x4::rotate(_rotation).transpose() * Matrix4x4::translate(-_pos);
    Vector3 start = inverseRot * other.start;
    Vector3 dir = inverseRot * other.dir;

    Vector3 inverseDir = Vector3(1,1,1) / dir;
    Vector3 cubeMax = _scale*0.5f + _pos;
    Vector3 cubeMin = -_scale*0.5f + _pos;
    //Vector3 cubeMax = obj->scale*0.5f + obj->position;
    //Vector3 cubeMin = -obj->scale*0.5f + obj->position;

    Vector3 rayToMin = (cubeMin - start)*inverseDir;
    Vector3 rayToMax = (cubeMax - start)*inverseDir;
    using std::max; using std::min;
    float tMin = max(max(min(rayToMin.x, rayToMax.x), min(rayToMin.y, rayToMax.y)), min(rayToMin.z, rayToMax.z));
    float tMax = min(min(max(rayToMin.x, rayToMax.x), max(rayToMin.y, rayToMax.y)), max(rayToMin.z, rayToMax.z));

    // Ray is past cube
    if (tMax < 0)
        return RayResult();
    
    if (tMin > tMax)
        return RayResult();
    
    Vector3 intersection = other.dir*tMin+other.start;
    
    Vector3 normal;
    //Vector3 localIntersection = (dir*tMin+start - obj->position)/obj->scale;
    Vector3 localIntersection = (dir*tMin+start - _pos)/_scale;
    Vector3 absPoint = Vector3(abs(localIntersection.x), abs(localIntersection.y), abs(localIntersection.z));
    if (absPoint.x > absPoint.y)
    {
        if (absPoint.x > absPoint.z)
        {
            // left/right
            if (localIntersection.x > 0) normal = Vector3(1,0,0);
            else normal = Vector3(-1,0,0);
        }
        else
        {
            // forward/back
            if (localIntersection.z > 0) normal = Vector3(0,0,1);
            else normal = Vector3(0,0,-1);
        }
    }
    else if (absPoint.y > absPoint.z)
    {
        // top/bottom
        if (localIntersection.y > 0) normal = Vector3(0,1,0);
        else normal = Vector3(0,-1,0);
    }
    else
    {
        // forward/back
        if (localIntersection.z > 0) normal = Vector3(0,0,1);
        else normal = Vector3(0,0,-1);
    }
    //normal = Matrix3x3::rotate(obj->rotation)*normal;
    normal = Matrix3x3::rotate(_rotation)*normal;
    return RayResult(intersection, normal, tMin);
}
CollisionData CubeCollider::is_colliding(Collider *other)
{
    if (refreshAABB)
        fix_bounding_box();
    if (other->needs_refresh())
        other->fix_bounding_box();
    if (!aabb_test(other))
        return CollisionData();

    if (auto cube = dynamic_cast<CubeCollider*>(other))
    {   
        Matrix3x3 rot0 = Matrix3x3::rotate(_rotation);
        Matrix3x3 rot1 = Matrix3x3::rotate(cube->_rotation);
        //Matrix3x3 rot0 = Matrix3x3::rotate(obj->rotation);
        //Matrix3x3 rot1 = Matrix3x3::rotate(cube->obj->rotation);

        Vector3 axes[6]; // normals
        axes[0] = rot0*Vector3(0,1,0);
        axes[1] = rot0*Vector3(0,0,-1);
        axes[2] = rot0*Vector3(1,0,0);

        axes[3] = rot1*Vector3(0,1,0);
        axes[4] = rot1*Vector3(0,0,-1);
        axes[5] = rot1*Vector3(1,0,0);

        float minDot[2], maxDot[2];
        std::vector<Vector3> points[2];
        get_points(points[0], this);
        get_points(points[1], cube);

        // direction & length 'this' has to move in order to not collide with 'cube'
        Vector3 mtv;
        float mtvMag = Math::INF;
        // Axes
        for (int i = 0; i < 6; ++i)
        {
            // Cube points
            for (int j = 0; j < 2; ++j)
            {
                minDot[j] = Vector3::dot(points[j][0], axes[i]);
                maxDot[j] = minDot[j];
                // Projections
                for (int k = 1; k < points[j].size(); ++k)
                {
                    float product = Vector3::dot(points[j][k], axes[i]);
                    if (minDot[j] > product)
                        minDot[j] = product;
                    
                    if (maxDot[j] < product)
                        maxDot[j] = product;
                }
            }
            /*
                tests
                <-min0--max0----min1--max1----> 0   correct
                <-min0--min1----max0--max1----> 1   correct
                <-min1--max1----min0--max0----> 0   correct
                <-min1--min0----max1--max0----> 1   correct
            */
            // Use projections
            if ((minDot[1]>=maxDot[0])
                || (minDot[0]>=maxDot[1]))
            {
                return CollisionData();
            }
            else
            {
                float overlap = (maxDot[0] < maxDot[1]) ? (minDot[1]-maxDot[0]) : -(minDot[0]-maxDot[1]);
                if (abs(mtvMag) > abs(overlap))
                {
                    mtvMag = overlap;
                    mtv = axes[i]*(mtvMag);
                }
            }
        }
        return CollisionData(mtv);
    }
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        auto data = sphere_cube_collision(sphere, this);
        data.dir = -data.dir;
        return data;
    }
    fprintf(stderr, "error: collision not implemented\n");
    return CollisionData();
}
void CubeCollider::fix_bounding_box()
{
    _pos = obj->position;
    _scale = obj->scale;
    _rotation = obj->rotation;

    std::vector<Vector3> points;
    get_points(points, this);
    aabbMin = Vector3();
    aabbMax = Vector3();
    for (int i = 0; i < points.size(); ++i)
    {
        Vector3 &point = points[i];
        using std::min; using std::max;
        aabbMin.x = min(point.x, aabbMin.x);
        aabbMin.y = min(point.y, aabbMin.y);
        aabbMin.z = min(point.z, aabbMin.z);

        aabbMax.x = max(point.x, aabbMax.x);
        aabbMax.y = max(point.y, aabbMax.y);
        aabbMax.z = max(point.z, aabbMax.z);
    }
    refreshAABB = 0;
}
void CubeCollider::refresh()
{
    refreshAABB = 1;
}