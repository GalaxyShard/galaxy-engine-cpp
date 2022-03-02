#pragma once

#define CLASS_LAMBDA(func) [this](){func();}
#define TYPE_LAMBDA(func, T) [this](T v){func(v);}
class Callback
{
private:
    char raw[8];
    void (*indirection)(char*);
public:
    Callback();
    Callback(std::nullptr_t);
    template<typename T>
    Callback(T func_or_lambda);
    void operator()();
    operator bool() const;
};
template<typename... Args>
class ArgCallback
{
private:
    char raw[8];
    void (*indirection)(char*, Args...);
public:
    ArgCallback();
    ArgCallback(std::nullptr_t);
    template<typename U>
    ArgCallback(U func_or_lambda);
    //void operator()(T data);
    void operator()(Args ...data);
    operator bool() const;
};
#include "callback.inl"