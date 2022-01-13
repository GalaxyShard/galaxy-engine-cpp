#include <Galaxy/Math/vector.hpp>
#include <cmath>
#include <ostream>
#define DEFINE_OP_BASE(t0, t1, op, r1, r2) \
    t0 t0::operator op(const t1 &v2) const { \
        return t0(x op r1, y op r2); \
    }
#define DEFINE_ALL() DEFINE_OP(+);DEFINE_OP(-);DEFINE_OP(*);DEFINE_OP(/);
#define DEFINE_OP(op) DEFINE_OP_BASE(Vector2, Vector2, op, v2.x, v2.y);
DEFINE_ALL();

#undef DEFINE_OP
#define DEFINE_OP(op) DEFINE_OP_BASE(Vector2, float, op, v2, v2);
DEFINE_ALL()


#undef DEFINE_OP
#define DEFINE_OP(op) DEFINE_OP_BASE(Vector2Int, Vector2Int, op, v2.x, v2.y);
DEFINE_ALL()

#undef DEFINE_OP
#define DEFINE_OP(op) DEFINE_OP_BASE(Vector2Int, int, op, v2, v2);
DEFINE_ALL()

#undef DEFINE_OP
#undef DEFINE_OP_BASE
#undef DEFINE_ALL

Vector2Int::Vector2Int(int x, int y) : x(x), y(y) { }
Vector2Int Vector2Int::operator -() { return Vector2Int(-x, -y); }
bool Vector2Int::operator ==(const Vector2Int &v2) const { return x == v2.x && y == v2.y; }


Vector2Int::operator Vector2() const { return Vector2(x, y); }
Vector2Int::operator Vector3() const { return Vector3(x, y, 0); }
Vector2Int::operator Vector4() const { return Vector4(x, y, 0, 0); }

std::ostream& operator<<(std::ostream &os, const Vector2Int &v)
{ return os << v.x << ", " << v.y; }



Vector2::Vector2(float x, float y) : x(x), y(y) { }
Vector2 Vector2::operator -() { return Vector2(-x, -y); }
bool Vector2::operator ==(const Vector2 &v2) const { return x == v2.x && y == v2.y; }

float Vector2::dot(const Vector2 &a, const Vector2 &b) { return a.x*b.x + a.y*b.y; }
float Vector2::sqr_magnitude() const { return x*x + y*y; }
float Vector2::magnitude() const { return std::sqrt(sqr_magnitude()); }


//Vector2::operator Vector2() const { return Vector2(x, y, 0, 0); }
Vector2::operator Vector3() const { return Vector3(x, y, 0); }
Vector2::operator Vector4() const { return Vector4(x, y, 0, 0); }

std::ostream& operator<<(std::ostream &os, const Vector2 &v)
{ return os << v.x << ", " << v.y; }