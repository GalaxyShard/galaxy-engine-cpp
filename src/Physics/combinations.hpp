#pragma once
#include <Galaxy/Physics/physics.hpp>

std::vector<Vector3> get_points(CubeCollider *cube);
CollisionData sphere_cube_collision(SphereCollider *sphere, CubeCollider *cube);
