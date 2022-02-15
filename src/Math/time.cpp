#include <Galaxy/Math/time.hpp>
#include <chrono>
float Time::mDelta = 0;
float Time::mRawDelta = 0;
//double Time::get()
//{
//    using namespace std::chrono;
//    return duration<double>(steady_clock::now().time_since_epoch()).count();
//}
//double Time::unix_epoch()
//{
//    using namespace std::chrono;
//    return duration<double>(system_clock::now().time_since_epoch()).count();
//}
//float mDelta = 1.f/60.f;
//float mRawDelta = mDelta;
//float Time::delta() { return mDelta; }
//float Time::raw_delta() { return mDelta; }
//float Time::delta = 0.0166f;
//float Time::smoothDelta = 0.0166f;
void Time::update_delta()
{
    using namespace std::chrono;

    const float maxDelta = 0.1f;
    static double lastTime = Time::get();
    mDelta = Time::get() - lastTime;
    mRawDelta = mDelta;
    if (mDelta > maxDelta) mDelta = maxDelta;
    lastTime = Time::get();

    //smoothDelta = (smoothDelta + delta)/2;

}