#pragma once
class Time
{
protected:
    static float mDelta, mRawDelta, mFrameTime;
public:
    static inline float delta();
    static inline float raw_delta();
    static inline float frame_time();
    static inline double get();
    static inline double unix_epoch();
};
#include "time.inl"