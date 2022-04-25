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
    constexpr inline float min(float a, float b) { return (a < b) ? a : b; }
    constexpr inline float max(float a, float b) { return (a > b) ? a : b; }
    constexpr inline float clamp(float v, float min, float max)
    {
        if (v > max) return max;
        else if (v < min) return min;
        return v;
    }
    constexpr inline float sqr(float v) { return v*v; }

    constexpr inline float remap(float v, float min, float max, float newMin, float newMax)
    { return (v-min)/(max-min)*(newMax-newMin)+newMin; }

    constexpr inline float lerp(float start, float goal, float t)
    { return start + (goal - start) * t; }


    template <typename T>
    void insertion_sort(std::vector<T> &array, bool(*predicate)(T &a, T &b));

    template<typename T>
    constexpr inline bool within(T v, T min, T max) { return v >= min && v <= max; }

    template<typename T>
    constexpr inline T abs(T v) { return v < (T)0 ? -v : v; }
    template<typename T>
    constexpr inline bool approx(T a, T b, T threshold = 0.00001f) { return abs(a-b)<=threshold; }


    constexpr float PI = 3.14159265359f;
    constexpr float INF = std::numeric_limits<float>::infinity();

    constexpr float to_deg = 180.0f / PI;
    constexpr float to_rad = PI / 180.0f;
}
template <typename T>
void Math::insertion_sort(std::vector<T> &array, bool(*predicate)(T &a, T &b))
{
    for (unsigned int i = 1; i < array.size(); ++i)
    {
        if (predicate(array[i], array[i-1]))
        {
            T element = array[i];
            int j = i-1;
            while (j >= 0 && predicate(element, array[j]))
            {
                array[j+1] = array[j];
                --j;
            }
            array[j+1] = element;
        }
    }
}