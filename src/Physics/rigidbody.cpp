#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/time.hpp>

std::vector<Rigidbody*> Rigidbody::allRigidbodies;
std::vector<Collider*> Collider::allColliders;
void Physics::simulate()
{
    const float maxDelta = 0.1f;
    
    static double lastTime = Time::get();
    float delta = Time::get() - lastTime;
    if (delta > maxDelta) delta = maxDelta;
    lastTime = Time::get();
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
        Collider *collider = body->obj->get_component<Collider>();
        body->obj->position += body->velocity * delta;
        if (!body->freezeRotation)
            body->obj->rotation += body->angularVelocity;
        
        if (!collider)
            continue;
        
        for (Collider *other : Collider::allColliders)
        {
            if (other == collider)
                continue;
            CollisionData data = collider->is_colliding(other);
            if (data.isColliding)
            {
                if (Rigidbody *otherRB = other->obj->get_component<Rigidbody>())
                {
                    //otherRB->velocity += ;
                }
                body->obj->position += data.dir;
                // temporary workaround for gravity
                //if (data.dir.y > 0)
                    //body->velocity.y = -0.1f;
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

    ObjComponent::~ObjComponent();
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
    ObjComponent::~ObjComponent();
}

void Rigidbody::add_force(Vector3 force)
{
    velocity += force / mass;
}
void Rigidbody::add_acceleration(Vector3 acceleration)
{
    velocity += acceleration;
}