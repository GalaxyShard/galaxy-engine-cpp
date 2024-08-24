#include <Galaxy/Math/matrix.hpp>
#include <math.h>

Matrix2x2::Matrix2x2(float v0, float v1, float v2, float v3)
    : v{Vector2(v0,v1),Vector2(v2,v3)} { }
Matrix2x2::Matrix2x2(Vector2 v0, Vector2 v1) : v{v0,v1} { }
Matrix2x2::Matrix2x2(Vector2 v[2]) : v{v[0],v[1]} { }

Vector2& Matrix2x2::operator[](int i) { return v[i]; }
const Vector2& Matrix2x2::operator[](int i) const { return v[i]; }
const float* Matrix2x2::value_ptr() const { return &v[0].x; }
Matrix2x2 Matrix2x2::rotate(float z)
{
    float o = sinf(z), a = cosf(z);
    return Matrix2x2(
        a, -o,
        o,  a
    );
}

Matrix2x2 Matrix2x2::identity()
{
    return Matrix2x2(
        1, 0,
        0, 1
    );
}
Matrix2x2 Matrix2x2::transpose() const
{
    return Matrix2x2(
        v[0][0], v[1][0],
        v[0][1], v[1][1]
    );
}
Vector2 Matrix2x2::operator*(const Vector2 &vb) const
{
    return Vector2(
        v[0][0]*vb[0] + v[0][1]*vb[1], 
        v[1][0]*vb[0] + v[1][1]*vb[1]
    );
}