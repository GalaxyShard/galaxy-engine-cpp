#include <Galaxy/Math/matrix.hpp>
#include <ostream>
#include <cmath>
Matrix3x3::Matrix3x3(float v0, float v1, float v2, float v3, float v4,
    float v5, float v6, float v7, float v8)
    : v{Vector3(v0, v1, v2), Vector3(v3, v4, v5), 
        Vector3(v6, v7, v8)} { }
Matrix3x3::Matrix3x3(Vector3 v0, Vector3 v1, Vector3 v2)
    : v{v0, v1, v2} { }
Matrix3x3::Matrix3x3(Vector3 *v) : v{v[0], v[1], v[2]} { }

Vector3& Matrix3x3::operator[](int i) { return v[i]; }
const Vector3& Matrix3x3::operator[](int i) const { return v[i]; }
const float* Matrix3x3::value_ptr() const { return &v[0].x; }

// Float operators
#define DEFINE_OP(OP) Matrix3x3 Matrix3x3::operator OP(const float &vb) const \
{ \
    return Matrix3x3( \
        v[0] OP vb, \
        v[1] OP vb, \
        v[2] OP vb \
    ); \
}
DEFINE_OP(+);
DEFINE_OP(-);
DEFINE_OP(*);
Matrix3x3 Matrix3x3::operator/(const float &vb) const { return this[0]*(1/vb); }

// Vector operators
Vector3 Matrix3x3::operator*(const Vector3 &vb) const
{
/*
    current memory layout
    00,01,02,03,10,11,12,13,20,21,22,23,30,31,32,33

    row vectors
        0   1   2   3
    0  [00, 01, 02, 03]
    1  [10, 11, 12, 13]
    2  [20, 21, 22, 23]
    3  [30, 31, 32, 33]

       [0]
       [1]
       [2]
       [3]
*/
    return Vector3(
        v[0][0]*vb[0] + v[0][1]*vb[1] + v[0][2]*vb[2],
        v[1][0]*vb[0] + v[1][1]*vb[1] + v[1][2]*vb[2],
        v[2][0]*vb[0] + v[2][1]*vb[1] + v[2][2]*vb[2]
    );
}

// Matrix operators
Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &vb) const
{
    //assert(false);
    //return 0;
    /*
        0   1   2   3
    0  [00, 01, 02, 03]
    1  [10, 11, 12, 13]
    2  [20, 21, 22, 23]
    3  [30, 31, 32, 33]

    [00*00 + 01*10 + 02*20 + 03*30,   00*01 + 01*11 + 02*21 + 03*31,   00*02 + 01*12 + 02*22 + 03*32,   00*03 + 01*13 + 02*23 + 03*33]
    [10*00 + 11*10 + 12*20 + 13*30,   10*01 + 11*11 + 12*21 + 13*31,   10*02 + 11*12 + 12*22 + 13*32,   10*03 + 11*13 + 12*23 + 13*33]
    [20*00 + 21*10 + 22*20 + 23*30,   20*01 + 21*11 + 22*21 + 23*31,   20*02 + 21*12 + 22*22 + 23*32,   20*03 + 21*13 + 22*23 + 23*33]
    [30*00 + 31*10 + 32*20 + 33*30,   30*01 + 31*11 + 32*21 + 33*31,   30*02 + 31*12 + 32*22 + 33*32,   30*03 + 31*13 + 32*23 + 33*33]
    
    */
    return Matrix3x3(
        v[0][0]*vb[0][0] + v[0][1]*vb[1][0] + v[0][2]*vb[2][0],
        v[0][0]*vb[0][1] + v[0][1]*vb[1][1] + v[0][2]*vb[2][1],
        v[0][0]*vb[0][2] + v[0][1]*vb[1][2] + v[0][2]*vb[2][2],
        
        v[1][0]*vb[0][0] + v[1][1]*vb[1][0] + v[1][2]*vb[2][0],
        v[1][0]*vb[0][1] + v[1][1]*vb[1][1] + v[1][2]*vb[2][1],
        v[1][0]*vb[0][2] + v[1][1]*vb[1][2] + v[1][2]*vb[2][2],
        
        v[2][0]*vb[0][0] + v[2][1]*vb[1][0] + v[2][2]*vb[2][0],
        v[2][0]*vb[0][1] + v[2][1]*vb[1][1] + v[2][2]*vb[2][1],
        v[2][0]*vb[0][2] + v[2][1]*vb[1][2] + v[2][2]*vb[2][2]
    );
}
Matrix3x3 Matrix3x3::identity()
{
    return Matrix3x3(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    );
}
Matrix3x3 Matrix3x3::transpose() const
{
    return Matrix3x3(
        v[0][0], v[1][0], v[2][0],
        v[0][1], v[1][1], v[2][1],
        v[0][2], v[1][2], v[2][2]
    );
}
Matrix3x3 Matrix3x3::inverse() const
{
    assert(false);
    return 0;
}
// rotation functions are based off of wikipedia, the order is from blender & trial/error
Matrix3x3 rotateX(float x)
{
    float o = sinf(x), a = cosf(x);
    return Matrix3x3(
        1, 0,  0,
        0, a, -o,
        0, o,  a
    );
}
Matrix3x3 rotateY(float y)
{
    float o = sinf(y), a = cosf(y);
    return Matrix3x3(
         a, 0, o,
         0, 1, 0,
        -o, 0, a
    );
}
Matrix3x3 rotateZ(float z)
{
    float o = sinf(z), a = cosf(z);
    return Matrix3x3(
        a, -o, 0,
        o,  a, 0,
        0,  0, 1
    );
}
Matrix3x3 Matrix3x3::rotate(float x, float y, float z)
{
    return rotateZ(z) * rotateY(y) * rotateX(x);
}
Matrix3x3 Matrix3x3::rotate(Vector3 v) { return rotate(v.x, v.y, v.z); }

//Matrix3x3 Matrix3x3::rotate_around(Vector3 point, Vector3 v)
//{ return rotate(v.x, v.y, v.z); }

std::ostream& operator<<(std::ostream &os, const Matrix3x3 &v)
{ return os << v[0] << "\n" << v[1] << "\n" << v[2]; }