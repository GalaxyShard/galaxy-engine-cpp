#include <Galaxy/Math/vector.hpp>
#include <ostream>

Quaternion::Quaternion(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w) { }
Quaternion::operator Vector4() const { return Vector4(x, y, z, w); }

float& Quaternion::operator[](int i){ return (&x)[i]; }
const float& Quaternion::operator[](int i) const { return (&x)[i]; }

Quaternion Quaternion::identity() { return Quaternion(0, 0, 0, 1); }

std::ostream& operator<<(std::ostream &os, const Quaternion &v)
{ return os << v.x << ", " << v.y << ", " << v.z << ", " << v.w; }