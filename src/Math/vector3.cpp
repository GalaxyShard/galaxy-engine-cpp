#include <Galaxy/Math/vector.hpp>
#include <cmath>
#include <ostream>
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

// Float operators
//Vector3 Vector3::operator+(const float &v2)
//{
//    return Vector3(x + v2, y + v2, z + v2);
//}
//Vector3 Vector3::operator-(const float &v2)
//{
//    return Vector3(x - v2, y - v2, z - v2);
//}
//Vector3 Vector3::operator*(const float &v2)
//{
//    return Vector3(x * v2, y * v2, z * v2);
//}
//Vector3 Vector3::operator/(const float &v2)
//{
//    return Vector3(x / v2, y / v2, z / v2);
//}

// Vector operators
//Vector3 Vector3::operator+(const Vector3 &v2)
//{
//    return Vector3(x + v2.x, y + v2.y, z + v2.z);
//}


Vector3 Vector3::operator-() { return Vector3(-x, -y, -z); }
bool Vector3::operator ==(const Vector3 &v2) const { return x == v2.x && y == v2.y; }

float Vector3::sqr_magnitude() const { return x*x + y*y + z*z; }
float Vector3::magnitude() const { return std::sqrt(sqr_magnitude()); }

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
    if (sqrdist == 0 || sqrdist <= speed*speed) return goal;

    float inverseDist = 1.0f / std::sqrt(sqrdist);
    return pos + to * (inverseDist * speed);
}
Vector3::operator Vector2() const { return Vector2(x, y); }
Vector3::operator Vector4() const { return Vector4(x, y, z, 0); }

std::ostream& operator<<(std::ostream &os, const Vector3 &v)
{ return os << v.x << ", " << v.y << ", " << v.z; }
