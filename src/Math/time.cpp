#include "internaltime.hpp"
#include <chrono>

float Time::mDelta = 0;
float Time::mRawDelta = 0;
float Time::mFrameTime = 0;
static double lastTime;

void InternalTime::initialize()
{
    lastTime = Time::get();
}
void InternalTime::start_frame()
{
    using namespace std::chrono;
    constexpr float maxDelta = 0.1f;

    mDelta = Time::get() - lastTime;
    mRawDelta = mDelta;

    if (mDelta > maxDelta) mDelta = maxDelta;
    lastTime = Time::get();
}
void InternalTime::end_frame()
{
    mFrameTime = Time::get() - lastTime;
}