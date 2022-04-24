#include "frameQueuer.hpp"
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Events/event.hpp>

static std::vector<Callback> frameQueue;

void FrameQueuer::trigger()
{
    uintg s = frameQueue.size();
    if (s)
    {
        for (uintg i = 0; i < s; ++i)
            frameQueue[i]();
        frameQueue.clear();
    }
}

void FrameQueuer::queue_next_frame(Callback func)
{
    frameQueue.push_back(func);
}