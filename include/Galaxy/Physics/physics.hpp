#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/object.hpp>

class Collider
{
public:
    Vector3 pos;
    
    virtual void fill_params(Object *obj) = 0;
    virtual bool is_colliding(Collider *other) = 0;
};
class CubeCollider : public Collider
{
public:
    Vector3 scale = Vector3(1,1,1);
    void fill_params(Object *obj) override;
    bool is_colliding(Collider *other) override;
    
};
class SphereCollider : public Collider
{
public:
    float radius = 0;
    void fill_params(Object *obj) override;
    bool is_colliding(Collider *other) override;
};