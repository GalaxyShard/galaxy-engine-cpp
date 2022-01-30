#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/object.hpp>
#include <unordered_map>

struct Ray;
class Physics
{
public:
    static bool autoSimulate;
    static void raycast(const Ray &ray);
    static void simulate();
};
class Rigidbody : public ObjComponent
{
private:
    static std::vector<Rigidbody*> allRigidbodies;
    int id;
    friend class Physics;
public:
    Vector3 velocity;
    float mass = 1;
    float drag = 0.01;
    float angularDrag = 0.01;
    bool freezeRotation = 1;
    
    Rigidbody();
    ~Rigidbody() override;

    void add_force(Vector3 force);
    void add_acceleration(Vector3 acceleration);

    Rigidbody(const Rigidbody&);
    Rigidbody(Rigidbody&&);
    Rigidbody& operator=(const Rigidbody&);
    Rigidbody& operator=(Rigidbody&&);
};

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
    bool collided=0;
    RayResult() = default;
    RayResult(Vector3 pos, Vector3 normal, float dist);
};
class Collider : public ObjComponent
{
private:
    static std::vector<Collider*> allColliders;
    int id;
    friend class Physics;
public:
    Collider();
    ~Collider() override;
    virtual CollisionData is_colliding(Collider *other) = 0;
    virtual RayResult is_colliding(const Ray &other) = 0;
};
class CubeCollider : public Collider
{
public:
    CollisionData is_colliding(Collider *other) override;
    RayResult is_colliding(const Ray &other) override;
    
};
class SphereCollider : public Collider
{
public:
    CollisionData is_colliding(Collider *other) override;
    RayResult is_colliding(const Ray &other) override;
};