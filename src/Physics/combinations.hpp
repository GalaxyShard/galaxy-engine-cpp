#pragma once
#include <Galaxy/Physics/physics.hpp>

void get_points(std::vector<Vector3> &p, CubeCollider *cube);
void get_local_points(std::vector<Vector3> &p, CubeCollider *cube);
CollisionData sphere_cube_collision(SphereCollider *sphere, CubeCollider *cube);
void min_max_dot(std::vector<Vector3> &points, Vector3 axis,  float &min, float &max);
