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
    
    // Simulate rigidbodies
    for (Rigidbody *&body : Rigidbody::allRigidbodies)
    {
        Collider *collider = body->obj->get_component<Collider>();
        body->obj->position += body->velocity * delta;
        
        if (!collider)
            continue;
        
        for (Collider *other : Collider::allColliders)
        {
            if (other == collider)
                continue;
            CollisionData data = collider->is_colliding(other);
            if (data.isColliding)
            {
                body->obj->position += data.dir;
                //body->velocity += data.dir;
                //body->velocity = Vector3(0,0,0);
            }
            //else
            //    printf("Not colliding with %s\n", typeid(*other).name());
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
    velocity += force * mass;
}
void Rigidbody::add_acceleration(Vector3 acceleration)
{
    velocity += acceleration;
}