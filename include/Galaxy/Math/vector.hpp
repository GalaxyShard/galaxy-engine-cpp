#pragma once
#include "gmath.hpp"
#include <iosfwd>

struct Vector2;
struct Vector3;
struct Vector4;
struct Quaternion;

struct Vector2
{
    float x, y;
    Vector2(float x = 0, float y = 0);
    Vector2 operator -();
    float& operator[](int i){ return (&x)[i]; }
    const float& operator[](int i) const { return (&x)[i]; }

    DECLARE_MATH_OPERATORS(Vector2, Vector2)
    DECLARE_MATH_OPERATORS(Vector2, float)

    bool operator ==(const Vector2 &) const;
    bool operator !=(const Vector2 &v) const { return !(*this == v); }
    DEFINE_MATH_OPERATORS(Vector2)
    DEFINE_MATH_OPERATORS(float)


    operator Vector3() const;
    operator Vector4() const;

    static float dot(const Vector2 &a, const Vector2 &b);
    float sqr_magnitude() const;
    float magnitude() const;

    friend std::ostream& operator<<(std::ostream &, const Vector2 &);
};
struct Vector2Int
{
    int x, y;
    Vector2Int(int x = 0, int y = 0);
    Vector2Int operator -();
    int& operator[](int i){ return (&x)[i]; }
    const int& operator[](int i) const { return (&x)[i]; }

    DECLARE_MATH_OPERATORS(Vector2Int, Vector2Int)
    DECLARE_MATH_OPERATORS(Vector2Int, int)

    bool operator ==(const Vector2Int &) const;
    bool operator !=(const Vector2Int &v) const { return !(*this == v); }

    DEFINE_MATH_OPERATORS(Vector2Int)
    DEFINE_MATH_OPERATORS(int)

    operator Vector2() const;
    operator Vector3() const;
    operator Vector4() const;

    friend std::ostream& operator<<(std::ostream &, const Vector2 &);
};
struct Vector3
{
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0);
    Vector3 operator -() const;
    float& operator[](int i);
    const float& operator[](int i) const;

    DECLARE_MATH_OPERATORS(Vector3, Vector3)
    DECLARE_MATH_OPERATORS(Vector3, float)

    bool operator ==(const Vector3 &) const;
    bool operator !=(const Vector3 &v) const { return !(*this == v); }

    DEFINE_MATH_OPERATORS(Vector3)
    DEFINE_MATH_OPERATORS(float)

    float sqr_magnitude() const;
    float magnitude() const;
    Vector3 unit() const;

    static float dot(const Vector3 &a, const Vector3 &b);
    static Vector3 cross(const Vector3 &a, const Vector3 &b);
    static Vector3 lerp(const Vector3 &a, const Vector3 &b, float t);

    static Vector3 move_towards(Vector3 pos, Vector3 goal, float speed);

    operator Vector2() const;
    operator Vector4() const;

    friend std::ostream& operator<<(std::ostream &os, const Vector3 &v);
};
struct Vector4
{
    float x, y, z, w;
    float& operator[](int i);
    const float& operator[](int i) const;
    Vector4(float x = 0, float y = 0, float z = 0, float w = 0);

    DECLARE_MATH_OPERATORS(Vector4, Vector4)
    DECLARE_MATH_OPERATORS(Vector4, float)
    DEFINE_MATH_OPERATORS(Vector4)
    DEFINE_MATH_OPERATORS(float)

    bool operator ==(const Vector4 &) const;
    bool operator !=(const Vector4 &v) const { return !(*this == v); }

    float sqr_magnitude() const;
    float magnitude() const;
    Vector4 unit() const;

    operator Vector2() const;
    operator Vector3() const;

    friend std::ostream& operator<<(std::ostream &, const Vector4 &);
};
struct Quaternion
{
    float x, y, z, w;
    
    Quaternion(float x, float y, float z, float w);

    float& operator[](int i);
    const float& operator[](int i) const;
    operator Vector4() const;
    Quaternion operator *(const Quaternion &q);

    static Quaternion identity();

    friend std::ostream& operator<<(std::ostream &os, const Quaternion &v);
};