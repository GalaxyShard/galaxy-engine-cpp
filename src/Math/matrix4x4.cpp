#include <Galaxy/Math/matrix.hpp>
#include <ostream>
#include <cassert>

#include <cmath>
Matrix4x4::Matrix4x4(float v0, float v1, float v2, float v3, float v4,
    float v5, float v6, float v7, float v8, float v9, float v10, float v11,
    float v12, float v13, float v14, float v15)
    : v{Vector4(v0, v1, v2, v3), Vector4(v4, v5, v6, v7), 
        Vector4(v8, v9, v10, v11), Vector4(v12, v13, v14, v15)} { }
Matrix4x4::Matrix4x4(Vector4 v0, Vector4 v1, Vector4 v2, Vector4 v3)
    : v{v0, v1, v2, v3} { }
Matrix4x4::Matrix4x4(Vector4 *v) : v{v[0], v[1], v[2], v[3]} { }

Vector4& Matrix4x4::operator[](int i) { return v[i]; }
const Vector4& Matrix4x4::operator[](int i) const { return v[i]; }
const float* Matrix4x4::value_ptr() const { return &v[0].x; }

// Float operators
#define DEFINE_OP(OP) Matrix4x4 Matrix4x4::operator OP(const float &vb) const \
{ \
    return Matrix4x4( \
        v[0] OP vb, \
        v[1] OP vb, \
        v[2] OP vb, \
        v[3] OP vb  \
    ); \
}
DEFINE_OP(+);
DEFINE_OP(-);
DEFINE_OP(*);
Matrix4x4 Matrix4x4::operator/(const float &vb) const { return this[0]*(1/vb); }

// Vector operators
Vector4 Matrix4x4::operator*(const Vector4 &vb) const
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

       [00*0 + 01*1 + 02*2 + 03*3]
       [10*0 + 11*1 + 12*2 + 13*3]
       [20*0 + 21*1 + 22*2 + 23*3]
       [30*0 + 31*1 + 32*2 + 33*3]
*/

    return Vector4(
        v[0][0]*vb[0] + v[0][1]*vb[1] + v[0][2]*vb[2] + v[0][3]*vb[3],
        v[1][0]*vb[0] + v[1][1]*vb[1] + v[1][2]*vb[2] + v[1][3]*vb[3],
        v[2][0]*vb[0] + v[2][1]*vb[1] + v[2][2]*vb[2] + v[2][3]*vb[3],
        v[3][0]*vb[0] + v[3][1]*vb[1] + v[3][2]*vb[2] + v[3][3]*vb[3]
    );
}

// Matrix operators
Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &vb) const
{
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
    
    tests: all results correct
    a = 32, 90, 13, 50, 4, 77, 39, 63, 26, 96, 48, 18, 5, 66, 42, 45
    b = 59, 1, 46, 48, 3, 40, 31, 79, 23, 78, 54, 51, 88, 37, 59, 41
    ab = 6857, 6496, 7914, 11359, 6908, 8457, 8394, 10847, 4510, 8276, 7826, 12018, 5419, 7586, 7199, 9441
    ba = 3328, 12971, 5030, 6001, 1457, 11540, 6405, 6783, 2707, 16626, 8075, 9331, 4703, 19139, 7141, 9638

    d = 29, 52, 83, 23, 99, 2, 40, 29, 8, 47, 22, 56, 22, 36, 76, 45
    e = 6, 52, 82, 13, 85, 74, 2, 81, 20, 55, 73, 10, 73, 53, 36, 33
    de = 7933, 11140, 9369, 6178, 3681, 9033, 12086, 2806, 8571, 8072, 4372, 5979, 7997, 10373, 9044, 5447
    */
    return Matrix4x4(
        v[0][0]*vb[0][0] + v[0][1]*vb[1][0] + v[0][2]*vb[2][0] + v[0][3]*vb[3][0],
        v[0][0]*vb[0][1] + v[0][1]*vb[1][1] + v[0][2]*vb[2][1] + v[0][3]*vb[3][1],
        v[0][0]*vb[0][2] + v[0][1]*vb[1][2] + v[0][2]*vb[2][2] + v[0][3]*vb[3][2],
        v[0][0]*vb[0][3] + v[0][1]*vb[1][3] + v[0][2]*vb[2][3] + v[0][3]*vb[3][3],
        
        v[1][0]*vb[0][0] + v[1][1]*vb[1][0] + v[1][2]*vb[2][0] + v[1][3]*vb[3][0],
        v[1][0]*vb[0][1] + v[1][1]*vb[1][1] + v[1][2]*vb[2][1] + v[1][3]*vb[3][1],
        v[1][0]*vb[0][2] + v[1][1]*vb[1][2] + v[1][2]*vb[2][2] + v[1][3]*vb[3][2],
        v[1][0]*vb[0][3] + v[1][1]*vb[1][3] + v[1][2]*vb[2][3] + v[1][3]*vb[3][3],
        
        v[2][0]*vb[0][0] + v[2][1]*vb[1][0] + v[2][2]*vb[2][0] + v[2][3]*vb[3][0],
        v[2][0]*vb[0][1] + v[2][1]*vb[1][1] + v[2][2]*vb[2][1] + v[2][3]*vb[3][1],
        v[2][0]*vb[0][2] + v[2][1]*vb[1][2] + v[2][2]*vb[2][2] + v[2][3]*vb[3][2],
        v[2][0]*vb[0][3] + v[2][1]*vb[1][3] + v[2][2]*vb[2][3] + v[2][3]*vb[3][3],

        v[3][0]*vb[0][0] + v[3][1]*vb[1][0] + v[3][2]*vb[2][0] + v[3][3]*vb[3][0],
        v[3][0]*vb[0][1] + v[3][1]*vb[1][1] + v[3][2]*vb[2][1] + v[3][3]*vb[3][1],
        v[3][0]*vb[0][2] + v[3][1]*vb[1][2] + v[3][2]*vb[2][2] + v[3][3]*vb[3][2],
        v[3][0]*vb[0][3] + v[3][1]*vb[1][3] + v[3][2]*vb[2][3] + v[3][3]*vb[3][3]
    );
}
Matrix4x4 Matrix4x4::identity()
{
    return Matrix4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}
Matrix4x4 Matrix4x4::transpose() const
{
    return Matrix4x4(
        v[0][0], v[1][0], v[2][0], v[3][0],
        v[0][1], v[1][1], v[2][1], v[3][1],
        v[0][2], v[1][2], v[2][2], v[3][2],
        v[0][3], v[1][3], v[2][3], v[3][3]
    );
}
//Matrix4x4 Matrix4x4::inverse() const
//{
//    /*
//    2x2
//    float inverseDet = 1 / (v[0][0]*v[1][1] - v[0][1]*v[1][0]);
//
//    return Matrix2x2(
//        v[1][1], -v[0][1],
//        -v[1][0], v[0][0]
//    ) * inverseDet;
//
//
//
//    1, 2, 3,
//    4, 5, 6,
//    7, 8, 9
//
//
//    1, 4, 7
//    2, 5, 8
//    3, 6, 9
//
//
//    3x3
//    [00, 01, 02]
//    [10, 11, 12]
//    [20, 21, 22]
//
//    det
//    00*(11*22 - 12*21) -
//    01*(10*22 - 12*20) +
//    02*(10*21 - 11*20)
//
//
//
//    float inverseDet = v[0][0]*(v[1][1]*v[2][2] - v[1][2]*v[2][1])
//        - v[0][1]*(v[1][0]*v[2][2] - v[1][2]*v[2][0])
//        + v[0][2]*(v[1][0]*v[2][1] - v[1][1]*v[2][0])
//    */
//
//
//    assert(false);
//    return 0;
//}
/*
https://stackoverflow.com/questions/18404890/how-to-build-perspective-projection-matrix-no-api
General form of the Projection Matrix

uw = near/right, and uh = near/top,
where right is the coordinates of right clip plan and
top is the coordinates of top clip plane.

As the above perspective projection is symmetric,
so right = half of horizon width and top = half of vertical height,
then uw/uh = top/right = height/width = 1/aspect

cot(x) is cotangent, equal to 1/tan(x)

    uh = Cot( fov/2 ) == 1/Tan(fov/2)
    uw / uh = 1/aspect

    uw         0       0       0
    0        uh       0       0
    0         0      f/(f-n)  1
    0         0    -fn/(f-n)  0

*/
Matrix4x4 Matrix4x4::ortho(float left, float right, float bottom, float top, float zNear, float zFar, bool leftHanded)
{
    
    // sources: glm for NO/ZO, and https://gamedev.stackexchange.com/questions/81932/how-can-i-manually-construct-a-right-handed-orthographic-projection-matrix
    // difference between ZO and NO seem to be the near/far clipping planes
    // probably -1~1 for NO, which is OpenGL, and 0~1 for ZO, which is DirectX

    // prevent division by 0
    assert(zNear != zFar);
    assert(top != bottom);
    assert(left != right);

    float mul = leftHanded ? 1 : -1; // right handed is negative z

    Vector3 s = Vector3(
        2/(right-left),
        2/(top-bottom),
        mul*2/(zNear-zFar) // remove *2 for ZO
    );
    Vector3 t = Vector3(
        (left+right)/(left-right),
        (top+bottom)/(bottom-top),
        (zNear+zFar)/(zNear-zFar) // use '+ zFar' for NO, remove for ZO eg zNear/(zNear-zFar)
    );
    return Matrix4x4(
        s[0], 0,    0,    t[0],
        0,    s[1], 0,    t[1],
        0,    0,    s[2], t[2],
        0,    0,    0,    1
    );
    
    /*
        2/(r-l),     0,           0,                     0,
        0,           2/(t-b),     0,                     0,
        0,           0,           2/(near-far),          0,
        (l+r)/(l-r), (t+b)/(b-t), (near+far)/(near-far), 1
    */
    /*
        2/(r-l), 0,       0,            (l+r)/(l-r),
        0,       2/(t-b), 0,            (t+b)/(b-t),
        0,       0,       2/(near-far), (near+far)/(near-far),
        0,       0,       0,            1
    */
}
Matrix4x4 Matrix4x4::perspective(float fovy, float aspect, float near, float far, bool leftHanded)
{
    /*
         /|
        / |
       /  |
      /   | top
     /fov |
    /_____|
      near
    
    tan(fov) = top/near
    top = tan(fov)*near
    */
    float top = tanf(fovy/2)*near;
    float right = top*aspect;
    return frustum(-right, right, -top, top, near, far, leftHanded);
}
Matrix4x4 Matrix4x4::frustum(float left, float right, float bottom, float top, float near, float far, bool leftHanded)
{
    (void)leftHanded; // ignore for now
    return Matrix4x4(
        near*2.f/(right-left), 0,                     0,                     near*(left+right)/(left-right),
        0,                     near*2.f/(top-bottom), 0,                     near*(bottom+top)/(bottom-top),
        0,                     0,                     (near+far)/(near-far), 2.f*far*near/(near-far),
        0,                     0,                     -1.f,                    0
    );
}
    
Matrix4x4 Matrix4x4::translate(float x, float y, float z)
{
    return Matrix4x4(
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    );
}
Matrix4x4 Matrix4x4::scale(float x, float y, float z)
{
    return Matrix4x4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    );
}
Matrix4x4 Matrix4x4::rotate(float x, float y, float z)
{
    Matrix3x3 r = Matrix3x3::rotate(x,y,z);
    const float *ptr = r.value_ptr();
    return Matrix4x4(
        ptr[0], ptr[1], ptr[2], 0,
        ptr[3], ptr[4], ptr[5], 0,
        ptr[6], ptr[7], ptr[8], 0,
        0,      0,      0,      1
    );
}
Matrix4x4 Matrix4x4::rotate_around(Vector3 point, Vector3 angles)
{ return translate(point) * rotate(angles) * translate(-point); }


Matrix4x4 Matrix4x4::translate(Vector3 v) { return translate(v.x, v.y, v.z); }
Matrix4x4 Matrix4x4::scale(Vector3 v) { return scale(v.x, v.y, v.z); }
Matrix4x4 Matrix4x4::rotate(Vector3 v) { return rotate(v.x, v.y, v.z); }
std::ostream& operator<<(std::ostream &os, const Matrix4x4 &v)
{ return os << v[0] << "\n" << v[1] << "\n" << v[2] << "\n" << v[3]; }