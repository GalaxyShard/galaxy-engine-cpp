#pragma once
#include "gmath.hpp"
#include "vector.hpp"
/*
Some info on (column vs row)-major
https://community.khronos.org/t/structuring-a-matrix-library/70124
4x1: vector4
[1]
[2]
[5]
[1]


OpenGL uses column-major matrices, this is row-major
Transpose the matrix (swap rows w columns) to get OpenGL matrices

all matrices:
2x2, 2x3, 2x4
3x2, 3x3, 3x4
4x2, 4x3, 4x4

can be multiplied:
2x2, 3x2, 4x2  :  2x2, 2x3, 2x4, v2
2x3, 3x3, 4x3  :  3x2, 3x3, 3x4, v3
2x4, 3x4, 4x4  :  4x2, 4x3, 4x4, v4

only include operators +, -, /, ==, and != for the same type
have * operators for every type that can be multiplied
*/

// NOTE: The vectors in matrices are ROW vectors, not the standard column vectors.
struct Matrix2x2
{
    Matrix2x2(float v0, float v1, float v2, float v3);
    Matrix2x2(Vector2 v0, Vector2 v1);
    Matrix2x2(Vector2 v[2]);

    const Vector4& operator[](int) const;
    const float* value_ptr() const;
    Matrix2x2 inverse();
private:
    Vector2 vectors[2];
};
struct Matrix3x3
{
    Matrix3x3(float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8);
    Matrix3x3(Vector3 v0, Vector3 v1, Vector3 v2);
    Matrix3x3(Vector3 v[3]);

    Vector3& operator[](int);
    const Vector3& operator[](int) const;
    const float* value_ptr() const;

    static Matrix3x3 rotate(float x, float y, float z);
    static Matrix3x3 rotate(Vector3 v);
    //static Matrix3x3 rotate_around(Vector3 point, Vector3 angles);

    static Matrix3x3 identity();
    Matrix3x3 transpose() const;
    Matrix3x3 inverse() const;

    DECLARE_MATH_OPERATORS(Matrix3x3, Matrix3x3);
    DECLARE_MATH_OPERATORS(Matrix3x3, float);

    bool operator==(const Matrix3x3 &) const;
    DEFINE_NOT_EQ(Matrix3x3);

    Vector3 operator*(const Vector3 &) const;

    friend std::ostream& operator<<(std::ostream &, const Matrix3x3 &);
private:
    Vector3 v[3];
};
struct Matrix4x4
{
    Matrix4x4(float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9, float v10, float v11, float v12, float v13, float v14, float v15);
    Matrix4x4(Vector4 v0, Vector4 v1, Vector4 v2, Vector4 v3);
    Matrix4x4(Vector4 v[4]);

    Vector4& operator[](int);
    const Vector4& operator[](int) const;
    const float* value_ptr() const;

    static Matrix4x4 ortho(float left, float right, float bottom, float top, float zNear, float zFar, bool leftHanded);
    static Matrix4x4 perspective(float fov, float aspect, float near, float far, bool leftHanded);
    static Matrix4x4 frustum(float left, float right, float bottom, float top, float near, float far, bool leftHanded);
    
    static Matrix4x4 translate(float x, float y, float z);
    static Matrix4x4 translate(Vector3 v);

    static Matrix4x4 scale(float x, float y, float z);
    static Matrix4x4 scale(Vector3 v);

    static Matrix4x4 rotate(float x, float y, float z);
    static Matrix4x4 rotate(Vector3 v);
    static Matrix4x4 rotate_around(Vector3 point, Vector3 angles);

    static Matrix4x4 identity();
    Matrix4x4 transpose() const;
    Matrix4x4 inverse() const;

    DECLARE_MATH_OPERATORS(Matrix4x4, Matrix4x4);
    DECLARE_MATH_OPERATORS(Matrix4x4, float);

    bool operator==(const Matrix4x4 &) const;
    DEFINE_NOT_EQ(Matrix4x4);

    Vector4 operator*(const Vector4 &) const;

    friend std::ostream& operator<<(std::ostream &, const Matrix4x4 &);
private:
    Vector4 v[4];
};

/* multiplication of two matrices:
    NOTE: these are all indexed as column major

     the number of columns in matrix 1 has to be the same as the number of rows in matrix 2
     m,n  *  n,p  =  m,p
     3x4 * 4x2 = 3x2
     
     matrix sizes are represented by rows x columns, so 1x2 would be a [a, b] matrix
     multiply the rows by the columns then add their components
          eg r0 * c0 for first, then r0 * c1 for second,
          r1 * c0 for third, r1 * c1 for fourth

     multiplying matrices is not commutive, meaning xy isnt always equal to yx
     
     multiplying matrices is done through calculating the dot product of each row and column,
     first the dot of each column of y and the first row of x, then repeat with the next row of x
     
     dot product is |x| * |y|
     or more efficiently,
     x[0]*y[0] + x[1]*y[1] + x[2]*y[2]

     the identity matrix, I, is similar to reduced echolen form,
          all diagonal entries are ones, everything else is a zero. 'I' can have any size, eg 4x4, 3x3
          3x3 example: [1, 0, 0]
                       [0, 1, 0]
                       [0, 0, 1]
     xI and Ix are always equal to x

     [x0, x1]  *  [y0, y1]
     [x2, x3]  *  [y2, y3]
     ->
     [(x0*y0 + x1*y2), (x0*y1 + x1*y3)]
     [(x2*y0 + x3*y2), (x2*y1 + x3*y3)]

     2x3
     x = [1, 3, 6]
         [5, 2, 4]

     3x2
     y = [6, 3]
         [7, 8]
         [9, 1]

     z = xy

     2x2
     z = [1*6 + 3*7 + 6*9,   1*3 + 3*8 + 6*1]
         [5*6 + 2*7 + 4*9,   5*3 + 2*8 + 4*1]
     
     z = [6+21+54, 3+24+6]
         [30+14+36, 15+16+4]

     z = [81, 33]
         [80, 35]

    
    inverse
    notes: must be nxn matrix, square
        inverse is just swapping fractions, instead of 3/7 its 7/3,
        for non-fractions it is 1/x

    x = [2, 3]
        [4, 5]

    I = [1, 0]
        [0, 1]

    x * inverse(x) = inverse(x) * x = I
    inverse(x) = (1/|x|) * adjoint(x)


    first calculate determinate
    notes: for 3x3 and higher, all the determinates are added together
        with alternating signs, so (det0 - det1 + det2 - det3 + det4) etc
    2x2
        0   1
    0  [00, 01]
    1  [10, 11]

    00*11 - 10*01



    3x3
        0   1   2
    0  [00, 01, 02]
    1  [10, 11, 12]
    2  [20, 21, 22]
    ->
    11, 12
    21, 22
    &
    10, 12
    20, 22
    &
    10, 11
    20, 21

    equation (double checked with GLM code):
    00*(11*22 - 12*21) -
    01*(10*22 - 12*20) +
    02*(10*21 - 11*20)





    4x4
        0   1   2   3
    0  [00, 01, 02, 03]
    1  [10, 11, 12, 13]
    2  [20, 21, 22, 23]
    3  [30, 31, 32, 33]
    
    ->
    11, 12, 13
    21, 22, 23
    31, 32, 33
    ->
    22, 23
    32, 33
    &
    21, 23
    31, 33
    &
    21, 22
    31, 32

    11*(22*33 - 23*32)
    12*(21*33 - 23*31)
    13*(21*32 - 22*31)

    ->
    10, 12, 13
    20, 22, 23
    30, 32, 33
    ->
    22, 23
    32, 33
    &
    20, 23
    30, 33
    &
    20, 22
    30, 32

    10*(22*33 - 23*32)
    12*(20*33 - 23*30)
    13*(20*32 - 22*30)

    ->
    10, 11, 13
    20, 21, 23
    30, 31, 33
    ->
    21, 23
    31, 33
    &
    20, 23
    30, 33
    &
    20, 21
    30, 31

    10*(21*33 - 23*31)
    11*(20*33 - 23*30)
    13*(20*31 - 21*30)

    ->
    10, 11, 12
    20, 21, 22
    30, 31, 32
    ->
    21, 22
    31, 32
    &
    20, 22
    30, 32
    &
    20, 21
    30, 31

    10*(21*32 - 22*31)
    11*(20*32 - 22*30)
    12*(20*31 - 21*30)


    all:
    +00*(
        +11*(22*33 - 23*32)
        -12*(21*33 - 23*31)
        +13*(21*32 - 22*31)
    )
    -01*(
        +10*(22*33 - 23*32)
        -12*(20*33 - 23*30)
        +13*(20*32 - 22*30)
    )
    +02*(
        +10*(21*33 - 23*31)
        -11*(20*33 - 23*30)
        +13*(20*31 - 21*30)
    )
    -03*(
        +10*(21*32 - 22*31)
        -11*(20*32 - 22*30)
        +12*(20*31 - 21*30)
    )

    GLM (converted to row-major)
    22*33 - 23*32;
    21*33 - 23*31;
    21*32 - 22*31;
    12*33 - 13*32;
    11*33 - 13*31;
    11*32 - 12*31;
    12*23 - 13*22;
    11*23 - 13*21;
    11*22 - 12*21;
    02*33 - 03*32;
    01*33 - 03*31;
    01*32 - 02*31;
    02*23 - 03*22;
    01*23 - 03*21;
    01*22 - 02*21;
    02*13 - 03*12;
    01*13 - 03*11;
    01*12 - 02*11;

*/