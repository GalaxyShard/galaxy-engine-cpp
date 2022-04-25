#pragma once
#include <chrono>
class Time
{
protected:
    static float m_delta, m_rawDelta, m_frameTime;
    static std::chrono::steady_clock::time_point m_startTime;
public:
    static inline float delta();
    static inline float raw_delta();
    static inline float frame_time();
    static inline double get();
    static inline double unix_epoch();
};
#include "time.inl"