#include <Galaxy/Math/vector.hpp>
#include <ostream>
Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }


float& Vector4::operator[](int i) { return (&x)[i]; }
const float& Vector4::operator[](int i) const { return (&x)[i]; }

bool Vector4::operator==(const Vector4 &v) const
{ return this->x == v.x && this->y == v.y && this->z == v.z && this->w == v.w; }

std::ostream& operator<<(std::ostream &os, const Vector4 &v)
{ return os << v.x << ", " << v.y << ", " << v.z << ", " << v.w; }

// Float operators
#define DEFINE_OP_BASE(t1, op, r1, r2, r3, r4) \
    Vector4 Vector4::operator op(const t1 &v2) const { \
        return Vector4(x op r1, y op r2, z op r3, w op r4); \
    }

#define DEFINE_OP(op) DEFINE_OP_BASE(Vector4, op, v2.x, v2.y, v2.z, v2.w);
DEFINE_OP(+);
DEFINE_OP(-);
DEFINE_OP(*);
DEFINE_OP(/);

#undef DEFINE_OP
#define DEFINE_OP(op) DEFINE_OP_BASE(float, op, v2, v2, v2, v2);
DEFINE_OP(+);
DEFINE_OP(-);
DEFINE_OP(*);
DEFINE_OP(/);
#undef DEFINE_OP
#undef DEFINE_OP_BASE


Vector4::operator Vector2() const { return Vector2(x, y); }
Vector4::operator Vector3() const { return Vector3(x, y, z); }
