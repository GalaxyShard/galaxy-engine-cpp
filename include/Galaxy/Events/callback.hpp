#pragma once

class EmptyCallbackClass{};
struct RawCallback
{
    typedef void (*func)();
    typedef void (EmptyCallbackClass::*member)();
    void *inst=0;
    union
    {
        func voidFunc;
        struct
        {
            member classFunc;
            void (*memberLambda)(void *inst, member classFunc);
        };
    };
};
class Callback
{
private:
    RawCallback raw;
public:
    Callback();
    Callback(void (*func)());
    template<typename T>
    Callback(T *inst, void (T::*func)());
    void operator()();
};


template <typename T>
struct RawArgCallback
{
    typedef void (*func)(T);
    typedef void (EmptyCallbackClass::*member)(T);
    void *inst=0;
    union
    {
        func voidFunc;
        struct
        {
            member classFunc;
            void (*memberLambda)(void *inst, member classFunc, T arg);
        };
    };
};
template <typename U>
class ArgCallback
{
private:
    //typedef RawArgCallback<U>::func raw_func;
    //typedef RawArgCallback<U>::member raw_member;
    typedef void (*raw_func)(U);
    typedef void (EmptyCallbackClass::*raw_member)(U);
    //typedef void (*func)(U);
    //typedef void (EmptyClass::*member)(U);
    RawArgCallback<U> raw;
    //RawCallback raw;
public:
    ArgCallback();
    ArgCallback(void (*func)(U));
    template<typename T>
    ArgCallback(T *inst, void (T::*func)(U));
    void operator()(U arg);
};
#include "callback.inl"