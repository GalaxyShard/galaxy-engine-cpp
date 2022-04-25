#include "internaltime.hpp"
#include <chrono>

float Time::m_delta = 0;
float Time::m_rawDelta = 0;
float Time::m_frameTime = 0;
std::chrono::steady_clock::time_point Time::m_startTime;
static double lastTime;

void InternalTime::initialize()
{
    lastTime = Time::get();
    m_startTime = std::chrono::steady_clock::now();
}
void InternalTime::start_frame()
{
    using namespace std::chrono;
    constexpr float maxDelta = 0.1f;

    m_delta = (float)(Time::get() - lastTime);
    m_rawDelta = m_delta;

    if (m_delta > maxDelta) m_delta = maxDelta;
    lastTime = Time::get();
}
void InternalTime::end_frame()
{
    m_frameTime = (float)(Time::get() - lastTime);
}