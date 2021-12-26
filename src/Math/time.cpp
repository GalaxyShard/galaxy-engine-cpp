#include <Galaxy/Math/time.hpp>
#include <debug.hpp>
#include <chrono>
double Time::get()
{
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count(); 
    
    //return unix_epoch();
    //using namespace std::chrono;
    //return ((duration<float>)steady_clock::now().time_since_epoch()).count();
    
    //duration<float> t = steady_clock::now().time_since_epoch();
    //return t.count();

    //#if OS_MOBILE
    //    return glfmGetTime();
    //#else
    //    return glfwGetTime();
    //#endif
}
double Time::unix_epoch()
{
    //std::chrono::steady_clock::duration;
    //std::chrono::nanoseconds;
    //std::chrono::duration<long long, std::nano>
    using namespace std::chrono;
    return duration<double>(system_clock::now().time_since_epoch()).count();
    //return duration<long long>(steady_clock::now().time_since_epoch()).count();
    //return (std::chrono::steady_clock::now().time_since_epoch()).count();
}
float Time::delta = 0.0166f;
float Time::smoothDelta = 0.0166f;
void Time::update_delta()
{
    //printf("epoch: %f, time: %d\n", unix_epoch(), (int)time(0));
    using namespace std::chrono;

    const float maxDelta = 0.1f;

    static auto lastTime = steady_clock::now();
    Time::delta = ((duration<float>)(steady_clock::now() - lastTime)).count();
    if (Time::delta > maxDelta) Time::delta = maxDelta;
    lastTime = steady_clock::now();

    //static double lastTime = Time::get();
    //delta = Time::get() - lastTime;
    //if (delta > maxDelta) delta = maxDelta;
    //lastTime = Time::get();

    smoothDelta = (smoothDelta + delta)/2;
    //printf("FPS: %f, smooth: %f\n", 1.f/delta, 1.f/smoothDelta);
}