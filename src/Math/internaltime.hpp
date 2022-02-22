#include <Galaxy/Math/time.hpp>
class InternalTime : private Time
{
public:
    static void initialize();
    static void start_frame();
    static void end_frame();
};