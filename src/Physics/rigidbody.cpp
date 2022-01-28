#include <Galaxy/Physics/physics.hpp>
#include <Galaxy/Math/time.hpp>

void Physics::simulate()
{
    const float maxDelta = 0.1f;
    
    static double lastTime = Time::get();
    float delta = Time::get() - lastTime;
    if (delta > maxDelta) delta = maxDelta;
    lastTime = Time::get();
    
    // Simulate rigidbodies here
}

void Rigidbody::add_force(Vector3 force)
{
    velocity += force * mass;
}
void Rigidbody::add_acceleration(Vector3 acceleration)
{
    velocity += acceleration;
}