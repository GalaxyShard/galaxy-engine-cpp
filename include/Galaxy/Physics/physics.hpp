#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/object.hpp>

struct CollisionData
{
    float penetration=0;
    Vector3 dir;
    bool isColliding=0;
    CollisionData(Vector3 dir, float penetration);
    CollisionData();
};

class Collider
{
public:
    Vector3 pos;
    
    virtual void fill_params(Object *obj) = 0;
    virtual CollisionData is_colliding(Collider *other) = 0;
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