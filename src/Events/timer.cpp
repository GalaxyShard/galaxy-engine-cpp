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
    for(uintg i = 0; i < timerCallbacks.size(); ++i)
    {
        if (time >= timerCallbacks[i].endTime)
        {
            timerCallbacks[i].c();
            timerCallbacks[i].c = nullptr;
        }
    }
}

uintg Timer::wait(double seconds, const Callback &callback)
{
    static bool didInit = 0;
    if (!didInit)
        Renderer::pre_render().connect_int(check_timers);
    didInit = 1;

    for(uintg i = 0; i < timerCallbacks.size(); ++i)
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
void Timer::cancel(uintg id)
{
    timerCallbacks.erase(timerCallbacks.begin()+id);
}