#pragma once
#include <vector>
#include <cstdint>

class Callback;
namespace Timer
{
    uint32_t wait(double seconds, const Callback &callback);
    void cancel(uint32_t id);
}