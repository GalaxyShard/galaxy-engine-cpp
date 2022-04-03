#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
#include <stdio.h>

#include "combinations.hpp"
#include <cmath>
Vector3 CubeCollider::scale() { return _scale; }
Vector3 CubeCollider::rotation() { return _rotation; }
RayResult CubeCollider::is_colliding(const Ray &other)
{
    // Axis-align box by rotating the ray around it
    Matrix3x3 rot = Matrix3x3::rotate(_rotation);
    Matrix3x3 inverseRot = Matrix3x3::rotate(_rotation).transpose();
    
    Vector3 start = inverseRot*(other.start-_pos) + _pos;
    Vector3 dir = inverseRot * other.dir;

    Vector3 inverseDir = Vector3(1,1,1) / dir;
    Vector3 cubeMax = _scale*0.5f + _pos;
    Vector3 cubeMin = -_scale*0.5f + _pos;

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
    // Normal calculations
    Vector3 normal;
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
    normal = rot*normal;
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

        float minDot[2], maxDot[2];
        std::vector<Vector3> points[2];
        get_points(points[0], this);
        get_points(points[1], cube);

        // direction & length 'this' has to move in order to not collide with 'cube'
        Vector3 mtv;
        float mtvMag = Math::INF;
        
        auto testAxis = [&](Vector3 axis) -> bool
        {
            // Cube points
            min_max_dot(points[0], axis, minDot[0], maxDot[0]);
            min_max_dot(points[1], axis, minDot[1], maxDot[1]);

            /*
                tests
                <-min0--max0----min1--max1----> 0   correct
                <-min1--max1----min0--max0----> 0   correct

                <-min0--min1----max0--max1----> 1   correct
                <-min1--min0----max1--max0----> 1   correct

                <-min0--min1----max1--max0----> 1   correct
                <-min1--min0----max0--max1----> 1   correct
            */
            if (minDot[1]<maxDot[0]
                && minDot[0]<maxDot[1])
            {
                // max1-min0   when 1 is less
                // min1-max0   when 0 is less
                float avg[2] = {(minDot[0]+maxDot[0])*0.5f, (minDot[1]+maxDot[1])*0.5f};
                float overlap = (avg[0] < avg[1]) ? minDot[1]-maxDot[0] : maxDot[1]-minDot[0];
                if (abs(overlap) < abs(mtvMag))
                {
                    mtvMag = overlap;
                    mtv = axis*mtvMag;
                }
                return 1;
            }
            return 0;
            //if ((minDot[1]>=maxDot[0])
            //    || (minDot[0]>=maxDot[1]))
            //{
            //    return false;
            //}
            //else
            //{
            //    float overlap = (maxDot[0] < maxDot[1]) ? (minDot[1]-maxDot[0]) : -(minDot[0]-maxDot[1]);
            //    if (abs(mtvMag) > abs(overlap))
            //    {
            //        mtvMag = overlap;
            //        mtv = axis*(mtvMag);
            //    }
            //}
            //return true;
        };
        #pragma region    Axis tests

        Vector3 up0 = rot0*Vector3(0,1,0);
        if (!testAxis(up0))
            return CollisionData();
        Vector3 right0 = rot0*Vector3(1,0,0);
        if (!testAxis(right0))
            return CollisionData();
        Vector3 forward0 = rot0*Vector3(0,0,1);
        if (!testAxis(forward0))
            return CollisionData();

        Vector3 up1 = rot1*Vector3(0,1,0);
        if (!testAxis(up1))
            return CollisionData();
        Vector3 right1 = rot1*Vector3(1,0,0);
        if (!testAxis(right1))
            return CollisionData();
        Vector3 forward1 = rot1*Vector3(0,0,1);
        if (!testAxis(forward1))
            return CollisionData();
        
        // causes errors (todo: check if it still does with the new implementation)
        //if (!testAxis(Vector3::cross(up0, up1))
        //    || !testAxis(Vector3::cross(up0, right1))
        //    || !testAxis(Vector3::cross(up0, forward1))
        //    || !testAxis(Vector3::cross(right0, up1))
        //    || !testAxis(Vector3::cross(right0, right1))
        //    || !testAxis(Vector3::cross(right0, forward1))
        //    || !testAxis(Vector3::cross(forward0, up1))
        //    || !testAxis(Vector3::cross(forward0, right1))
        //    || !testAxis(Vector3::cross(forward0, forward1)))
        //    return CollisionData();

        #pragma endregion

        return CollisionData(mtv);
    }
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        auto data = sphere_cube_collision(sphere, this);
        data.dir = -data.dir;
        return data;
    }
    logerr("error: collision not implemented\n");
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
    for (unsigned int i = 0; i < points.size(); ++i)
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