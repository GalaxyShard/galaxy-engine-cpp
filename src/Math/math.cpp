#include <Galaxy/Math/gmath.hpp>

float Math::min(float a, float b) { return (a < b) ? a : b; }
float Math::max(float a, float b) { return (a > b) ? a : b; }

float Math::clamp(float v, float min, float max)
{
    if (v > max) return max;
    else if (v < min) return min;
    return v;
}
float Math::lerp(float start, float goal, float t)
{
    return start + (goal - start) * t;
}
//float Math::sqr(float v) { return v*v; }