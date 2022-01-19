#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/matrix.hpp>
#include "combinations.hpp"
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
        Matrix3x3 rot0 = Matrix3x3::rotate(rotation);
        Matrix3x3 rot1 = Matrix3x3::rotate(cube->rotation);

        Vector3 axes[6]; // normals
        axes[0] = rot0*Vector3(0,1,0);
        axes[1] = rot0*Vector3(0,0,-1);
        axes[2] = rot0*Vector3(1,0,0);

        axes[3] = rot1*Vector3(0,1,0);
        axes[4] = rot1*Vector3(0,0,-1);
        axes[5] = rot1*Vector3(1,0,0);

        float minDot[2], maxDot[2];
        auto fillPoints = [&](std::vector<Vector3> &p, CubeCollider *cube)
        {
            Matrix3x3 rot = Matrix3x3::rotate(cube->rotation);
            Vector3 corner = cube->scale*0.5f;
            p.push_back(rot*(corner*Vector3(-1,-1,1))+cube->pos);
            p.push_back(rot*(corner*Vector3(1,-1,1))+cube->pos);
            p.push_back(rot*(corner*Vector3(-1,1,1))+cube->pos);
            p.push_back(rot*(corner*Vector3(1,1,1))+cube->pos);

            p.push_back(rot*(corner*Vector3(-1,-1,-1))+cube->pos);
            p.push_back(rot*(corner*Vector3(1,-1,-1))+cube->pos);
            p.push_back(rot*(corner*Vector3(-1,1,-1))+cube->pos);
            p.push_back(rot*(corner*Vector3(1,1,-1))+cube->pos);
        };
        std::vector<Vector3> points[2];
        fillPoints(points[0], this);
        fillPoints(points[1], cube);

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
                    {
                        minDot[j] = product;
                    }
                    if (maxDot[j] < product)
                    {
                        maxDot[j] = product;
                    }
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
                return 0;
            }
        }
        return 1;
    }
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        return sphere_cube_collision(sphere, this);
    }
    fprintf(stderr, "error: collision not implemented\n");
    return 0;
}