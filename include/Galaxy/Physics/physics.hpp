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
    Vector3 direction;
    Ray(Vector3 start, Vector3 direction);
};
struct RayResult
{
    float distance = 0;
    Object *object = 0;
    Vector3 normal;
    Vector3 position;
};

class Collider
{
public:
    Vector3 pos;
    
    virtual void fill_params(Object *obj) = 0;
    virtual CollisionData is_colliding(Collider *other) = 0;
    //virtual RayResult is_colliding(const Ray &other) = 0;
};
class CubeCollider : public Collider
{
public:
    Vector3 scale = Vector3(1,1,1);
    Vector3 rotation;
    void fill_params(Object *obj) override;
    CollisionData is_colliding(Collider *other) override;
    
};
class SphereCollider : public Collider
{
public:
    float radius = 0;
    void fill_params(Object *obj) override;
    CollisionData is_colliding(Collider *other) override;
};