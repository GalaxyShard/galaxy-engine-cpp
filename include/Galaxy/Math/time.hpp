#pragma once
namespace Time
{
    extern float delta, smoothDelta;
    double get();
    double unix_epoch();

    void update_delta(); // internal
}