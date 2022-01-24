#pragma once
#include <Galaxy/Physics/physics.hpp>

void get_points(std::vector<Vector3> &p, CubeCollider *cube);
void get_local_points(std::vector<Vector3> &p, CubeCollider *cube);
//std::vector<Vector3> get_points(CubeCollider *cube);
//std::vector<Vector3> get_local_points(CubeCollider *cube);
CollisionData sphere_cube_collision(SphereCollider *sphere, CubeCollider *cube);
