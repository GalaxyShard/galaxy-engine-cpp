inline double Time::get()
{
    using namespace std::chrono;
    return duration<double>(steady_clock::now()-m_startTime).count();
}
inline double Time::unix_epoch()
{
    using namespace std::chrono;
    return duration<double>(system_clock::now().time_since_epoch()).count();
}
inline float Time::delta() { return m_delta; }
inline float Time::raw_delta() { return m_rawDelta; }
inline float Time::frame_time() { return m_frameTime; }
