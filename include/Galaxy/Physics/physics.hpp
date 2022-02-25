#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/object.hpp>
#include <unordered_map>

struct Ray;
struct RayResult;
class Physics
{
public:
    static RayResult raycast(const Ray &ray);
    static void simulate(float delta);
};
class Rigidbody : public ObjComponent
{
private:
    static std::vector<Rigidbody*> allRigidbodies;
    int id;
    friend class Physics;
public:
    Vector3 velocity, angularVelocity;
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
    Vector3 pos;
    Vector3 normal;
    float dist = 0;
    bool collided=0;
    Object *object = 0;
    RayResult() = default;
    RayResult(Vector3 pos, Vector3 normal, float dist);
};
class Collider : public ObjComponent
{
private:
    static std::vector<Collider*> allColliders;
    int id;

    friend class Physics;
protected:
    Vector3 aabbMin, aabbMax;
    bool refreshAABB = 1;
    Vector3 _pos;
public:
    Vector3 pos();

    Collider();
    ~Collider() override;
    bool aabb_test(Collider *other);
    inline bool needs_refresh() { return refreshAABB; }
    virtual CollisionData is_colliding(Collider *other) = 0;
    virtual RayResult is_colliding(const Ray &other) = 0;
    virtual void fix_bounding_box() = 0;
    virtual void refresh() = 0;
};
class CubeCollider : public Collider
{
private:
    Vector3 _scale;
    Vector3 _rotation;
public:
    Vector3 scale();
    Vector3 rotation();

    CollisionData is_colliding(Collider *other) override;
    RayResult is_colliding(const Ray &other) override;
    void fix_bounding_box() override;
    void refresh() override;
};
class SphereCollider : public Collider
{
private:
    float _radius=0.5;
public:
    float radius();

    CollisionData is_colliding(Collider *other) override;
    RayResult is_colliding(const Ray &other) override;
    void fix_bounding_box() override;
    void refresh() override;
};