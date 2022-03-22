#include <Galaxy/Math/vector.hpp>
#include <ostream>

#include <cmath>
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) { }

float& Vector3::operator[](int i){ return (&x)[i]; }
const float& Vector3::operator[](int i) const { return (&x)[i]; }

#define DEFINE_OP_BASE(t1, op, r1, r2, r3) \
    Vector3 Vector3::operator op(const t1 &v2) const { \
        return Vector3(x op r1, y op r2, z op r3); \
    }

#define DEFINE_OP(op) DEFINE_OP_BASE(Vector3, op, v2.x, v2.y, v2.z);
DEFINE_OP(+);
DEFINE_OP(-);
DEFINE_OP(*);
DEFINE_OP(/);

#undef DEFINE_OP
#define DEFINE_OP(op) DEFINE_OP_BASE(float, op, v2, v2, v2);
DEFINE_OP(+);
DEFINE_OP(-);
DEFINE_OP(*);
DEFINE_OP(/);
#undef DEFINE_OP
#undef DEFINE_OP_BASE


Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z); }
bool Vector3::operator ==(const Vector3 &v) const
{ return Math::approx(x,v.x) && Math::approx(y,v.y) && Math::approx(z,v.z); }
//bool Vector3::operator ==(const Vector3 &v2) const { return x == v2.x && y == v2.y; }
/*
    graph: https://www.desmos.com/calculator/bgesi0457d
       /|
      / |
     /  |
    /___|____
      p   <- right angle
    or
        /
       /\  <- right angle
    p /  \
     /____\
    divide the dot product by the ||vector|| with the right angle (the one you are projecting on)
    to get the length of the projection on that vector

    in collision system there is no division bc the vector being projected on is a unit vector
        dot(a,b) == ||p||*||b||
        ||b|| == 1
        ||p|| == dot(a,b)*1
        ||p|| == dot(a,b)

    similar to sqr_magnitude vs magnitude
*/
float Vector3::dot(const Vector3 &a, const Vector3 &b)
{ return a.x*b.x + a.y*b.y + a.z*b.z; }
Vector3 Vector3::cross(const Vector3 &a, const Vector3 &b)
{
    return Vector3(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x);
}
float Vector3::sqr_magnitude() const { return x*x + y*y + z*z; }
float Vector3::magnitude() const { return std::sqrt(sqr_magnitude()); }

Vector3 Vector3::unit() const { return (*this) / magnitude(); }

Vector3 Vector3::lerp(const Vector3 &a, const Vector3 &b, float t)
{
    if (t > 1) return b;
    else if (t < 0) return a;
    return (b - a) * t + a;
}

Vector3 Vector3::move_towards(Vector3 pos, Vector3 goal, float speed)
{
    Vector3 to = goal - pos;
    float sqrdist = to.sqr_magnitude();
    if (sqrdist == 0.f || sqrdist <= speed*speed) return goal;

    float inverseDist = 1.f / std::sqrt(sqrdist);
    return pos + to * (inverseDist * speed);
}
Vector3::operator Vector2() const { return Vector2(x, y); }
Vector3::operator Vector4() const { return Vector4(x, y, z, 0.f); }

std::ostream& operator<<(std::ostream &os, const Vector3 &v)
{ return os << v.x << ", " << v.y << ", " << v.z; }
