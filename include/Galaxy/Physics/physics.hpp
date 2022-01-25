#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/object.hpp>
struct CollisionData
{
    Vector3 dir;
    bool isColliding=0;
    CollisionData(Vector3 mtv);
    CollisionData();
};
struct Ray
{
    Vector3 start;
    Vector3 dir;
    Ray(Vector3 start, Vector3 dir);
};
struct RayResult
{
    float dist = 0;
    Object *object = 0;
    Vector3 normal;
    Vector3 pos;
    RayResult() = default;
    RayResult(Vector3 pos, Vector3 normal, float dist);
};

// possibly rename Collider classes to ColliderData,
// then have actual collider classes as a part of objects
// also add a component system for objects to simplify things such as rigidbodies, colliders, meshes, materials, etc
class Collider
{
public:
    Vector3 pos;
    
    virtual void fill_params(Object *obj) = 0;
    virtual CollisionData is_colliding(Collider *other) = 0;
    virtual RayResult is_colliding(const Ray &other) = 0;
};
class CubeCollider : public Collider
{
public:
    Vector3 scale = Vector3(1,1,1);
    Vector3 rotation;
    void fill_params(Object *obj) override;
    CollisionData is_colliding(Collider *other) override;
    RayResult is_colliding(const Ray &other) override;
    
};
class SphereCollider : public Collider
{
public:
    float radius = 0;
    void fill_params(Object *obj) override;
    CollisionData is_colliding(Collider *other) override;
    RayResult is_colliding(const Ray &other) override;
};