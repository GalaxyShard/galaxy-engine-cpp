#pragma once
#include <vector>
class Callback;
namespace Timer
{
    uintg wait(double seconds, const Callback &callback);
    void cancel(uintg id);
}