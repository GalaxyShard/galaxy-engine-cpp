#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/time.hpp>

std::vector<Rigidbody*> Rigidbody::allRigidbodies;
std::vector<Collider*> Collider::allColliders;
RayResult Physics::raycast(const Ray &ray)
{
    RayResult result;
    result.dist = Math::INF;
    for (Collider *collider : Collider::allColliders)
    {
        RayResult next = collider->is_colliding(ray);
        if (next.collided && next.dist > 0 && next.dist < result.dist)
        {
            result = next;
            result.object = collider->obj;
        }
    }
    if (result.dist == Math::INF)
        return RayResult();
    return result;
}
void Physics::simulate(float delta)
{
/*
    https://www.toptal.com/game/video-game-physics-part-i-an-introduction-to-rigid-body-dynamics
    Moment of Inertia Tensor

    Solid sphere
    i = 0.4*mass*sqr(radius)
    [i, 0, 0]
    [0, i, 0]
    [0, 0, i]

    Solid cube
    [mass/12*(sqr(h) + sqr(d), 0,                         0]
    [0,                        mass/12*(sqr(w) + sqr(d)), 0]
    [0,                        0,                         mass/12*(sqr(w) + sqr(h))]

*/
    
    // Simulate rigidbodies
    for (Rigidbody *&body : Rigidbody::allRigidbodies)
    {
        body->obj->position += body->velocity * delta;
        body->add_acceleration(Vector3(0,-9.81f,0)*delta);
        body->velocity *= 1 - (body->drag*delta);
        if (!body->freezeRotation)
            body->obj->rotation += body->angularVelocity;
        
        Collider *collider = body->obj->get_component<Collider>();
        if (!collider)
            continue;
        
        collider->refresh();
        for (Collider *other : Collider::allColliders)
        {
            if (other == collider)
                continue;
            CollisionData data = collider->is_colliding(other);
            if (data.isColliding)
            {
                if (Rigidbody *otherRB = other->obj->get_component<Rigidbody>())
                {
                    Vector3 dir = data.dir*0.5;
                    otherRB->obj->position += -dir;
                    body->obj->position += dir;
                    other->refresh();
                    //otherRB->add_acceleration(-dir*Time::delta);
                    //body->add_acceleration(dir*Time::delta);
                }
                else
                {
                    body->obj->position += data.dir;
                    //body->add_acceleration(data.dir*Time::delta);
                }
                collider->refresh();
                // temporary workaround for gravity
                if (data.dir.y > 0)
                    body->velocity.y = -0.1f;
            }
        }
    }
}
Rigidbody::Rigidbody()
{
    id = allRigidbodies.size();
    allRigidbodies.push_back(this);
}
Rigidbody::~Rigidbody()
{
    Rigidbody *&back = allRigidbodies.back();
    back->id = id;
    std::swap(back, allRigidbodies[id]);
    allRigidbodies.pop_back();

    //ObjComponent::~ObjComponent();
}
Collider::Collider()
{
    id = allColliders.size();
    allColliders.push_back(this);
}
Collider::~Collider()
{
    Collider *&back = allColliders.back();
    back->id = id;
    std::swap(back, allColliders[id]);
    allColliders.pop_back();
    //ObjComponent::~ObjComponent();
}

void Rigidbody::add_force(Vector3 force)
{
    velocity += force / mass;
}
void Rigidbody::add_acceleration(Vector3 acceleration)
{
    velocity += acceleration;
}