#pragma once

#define CLASS_LAMBDA(func) [this](){func();}
#define TYPE_LAMBDA(func, T) [this](T v){func(v);}
class Callback
{
private:
    void *raw;
    void (*indirection)(void*);
public:
    Callback();
    Callback(std::nullptr_t);

    template<typename T>
    Callback(T func_or_lambda);

    void operator()() const;
    operator bool() const;
};
template<typename... Args>
class ArgCallback
{
private:
    void *raw;
    void (*indirection)(void*, Args...);
public:
    ArgCallback();
    ArgCallback(std::nullptr_t);

    template<typename T>
    ArgCallback(T func_or_lambda);
    
    void operator()(Args ...data) const;
    operator bool() const;
};
#include "callback.inl"