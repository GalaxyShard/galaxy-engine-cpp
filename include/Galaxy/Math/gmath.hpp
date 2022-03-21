#pragma once
#include <limits>
#include <vector>
#define DECLARE_MATH_OPERATORS(t1, t2) \
    t1 operator +(const t2 &) const; \
    t1 operator -(const t2 &) const; \
    t1 operator *(const t2 &) const; \
    t1 operator /(const t2 &) const;

#define DEFINE_MATH_OPERATORS(t) \
    void operator +=(const t &v2) { *this = *this + v2; } \
    void operator -=(const t &v2) { *this = *this - v2; } \
    void operator *=(const t &v2) { *this = *this * v2; } \
    void operator /=(const t &v2) { *this = *this / v2; }

namespace Math
{
    float min(float a, float b);
    float max(float a, float b);
    float clamp(float v, float min, float max);
    inline float sqr(float v) { return v*v; }

    template <typename T>
    void insertion_sort(std::vector<T> &array, bool(*predicate)(T a, T b));

    template<typename T>
    bool within(T v, T min, T max) { return v >= min && v <= max; }

    float lerp(float start, float goal, float t);
    constexpr float PI = 3.14159265359f;
    constexpr float INF = std::numeric_limits<float>::infinity();

    constexpr float to_deg = 180.0f / PI;
    constexpr float to_rad = PI / 180.0f;
}
template <typename T>
void Math::insertion_sort(std::vector<T> &array, bool(*predicate)(T a, T b))
{
    for (unsigned int i = 1; i < array.size(); ++i)
    {
        if (predicate(array[i], array[i-1]))
        {
            T element = array[i];
            int j = i;
            while (j > 0 && predicate(element, array[j]))
            {
                --j;
                array[j+1] = array[j];
            }
            array[j] = element;
            //int j = i-1;
            //while (j >= 0 && predicate(element, array[j]))
            //{
            //    array[j+1] = array[j];
            //    --j;
            //}
            //array[j+1] = element;
        }
    }
}