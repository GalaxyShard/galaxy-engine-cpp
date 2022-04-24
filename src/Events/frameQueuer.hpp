#pragma once
#include <Galaxy/Events/callback.hpp>
class FrameQueuer
{
private:
    static void trigger();
    friend class Renderer;
public:
    static void queue_next_frame(Callback func);
};