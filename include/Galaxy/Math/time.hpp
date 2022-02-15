#pragma once
class Time
{
private:
    static float mDelta, mRawDelta;
public:
    static inline float delta();
    static inline float raw_delta();
    static inline double get();
    static inline double unix_epoch();

    static void update_delta(); // internal
};
#include "time.inl"