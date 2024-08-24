#include <Galaxy/Events/timer.hpp>
#include <Galaxy/Events/event.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Math/time.hpp>
struct TimerData
{
    Callback c;
    double endTime;
    TimerData(Callback c, double endTime) : c(c), endTime(endTime) {}
};
static std::vector<TimerData> timerCallbacks;
static void check_timers()
{
    double time = Time::get();
    for(uint32_t i = 0; i < timerCallbacks.size(); ++i)
    {
        if (time >= timerCallbacks[i].endTime)
        {
            timerCallbacks[i].c();
            timerCallbacks[i].c = nullptr;
        }
    }
}

uint32_t Timer::wait(double seconds, const Callback &callback)
{
    static bool didInit = 0;
    if (!didInit)
        Renderer::pre_render().connect_int(check_timers);
    didInit = 1;

    for(uint32_t i = 0; i < timerCallbacks.size(); ++i)
    {
        TimerData &data = timerCallbacks[i];
        if (!data.c)
        {
            data.endTime = Time::get()+seconds;
            data.c = callback;
            return i;
        }
    }

    timerCallbacks.emplace_back(callback, Time::get()+seconds);
    return timerCallbacks.size()-1;
}
void Timer::cancel(uint32_t id)
{
    timerCallbacks[id].c = nullptr;
}