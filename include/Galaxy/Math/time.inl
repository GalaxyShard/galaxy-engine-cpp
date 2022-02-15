
#include <chrono>

inline double Time::get()
{
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}
inline double Time::unix_epoch()
{
    using namespace std::chrono;
    return duration<double>(system_clock::now().time_since_epoch()).count();
}
inline float Time::delta() { return mDelta; }
inline float Time::raw_delta() { return mRawDelta; }
