#pragma once
#include "keycodes.hpp"
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/Events/callback.hpp>
//typedef void(*input_callback)(bool pressed);
typedef ArgCallback<bool> input_callback;
class Signal;

template<typename T>
class SignalT;

enum TouchState { PRESSED, RELEASED, MOVED };
struct TouchData
{
    int id;
    Vector2 pos, delta, startPos;
    TouchState state;
};

namespace Input
{
    void add_bind(const char *bind, KeyCode key);
    void add_bind(const char *bind, KeyCode key, input_callback callback);
    void remove_bind(const char *bind);
    bool is_held(const char *bind);
    void trigger(const char *bind, bool isPressed);
    void rebind(const char *bind, KeyCode newKey);
    void interactive_rebind(const char *bind, bool(*onFinish)(KeyCode key));

    extern Vector2 mousePos, mouseDelta;
    SignalT<TouchData>& touch_changed();
};