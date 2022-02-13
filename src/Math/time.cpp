#include <Galaxy/Math/time.hpp>
#include <gldebug.hpp>
#include <chrono>
double Time::get()
{
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}
double Time::unix_epoch()
{
    using namespace std::chrono;
    return duration<double>(system_clock::now().time_since_epoch()).count();
}
float Time::delta = 0.0166f;
float Time::smoothDelta = 0.0166f;
void Time::update_delta()
{
    using namespace std::chrono;

    const float maxDelta = 0.1f;
    static double lastTime = Time::get();
    delta = Time::get() - lastTime;
    if (delta > maxDelta) delta = maxDelta;
    lastTime = Time::get();

    smoothDelta = (smoothDelta + delta)/2;

}