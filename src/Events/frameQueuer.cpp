#include "frameQueuer.hpp"
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Events/event.hpp>

static std::vector<Callback> frameQueue;

void FrameQueuer::trigger()
{
    uint32_t s = frameQueue.size();
    if (s)
    {
        for (uint32_t i = 0; i < s; ++i)
            frameQueue[i]();
        frameQueue.clear();
    }
}

void FrameQueuer::queue_next_frame(Callback func)
{
    frameQueue.push_back(func);
}