#pragma once
#include <cstddef>

#define MK_LAMBDA(func) [this](){func();}
#define ARG_LAMBDA(func) [this](auto v0){func(v0);}
#define ARG_LAMBDA1(func) [this](auto v0){func(v0);}
#define ARG_LAMBDA2(func) [this](auto v0,auto v1){func(v0,v1);}
#define ARG_LAMBDA3(func) [this](auto v0,auto v1,auto v2){func(v0,v1,v2);}
#define ARG_LAMBDA4(func) [this](auto v0,auto v1,auto v2,auto v3){func(v0,v1,v2,v3);}


class Callback;
template<typename... Args>
class ArgCallback;
struct RawCallback
{
    void *data;
    void *indirection;
    explicit RawCallback(Callback c);
    template<typename... Args>
    explicit RawCallback(ArgCallback<Args...> c);
};
class Callback
{
private:
    void *raw;
    void (*indirection)(void*);
    friend struct RawCallback;
public:
    Callback();
    Callback(std::nullptr_t);

    Callback(void *raw, void *indirection);

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
    friend struct RawCallback;
public:
    ArgCallback();
    ArgCallback(std::nullptr_t);
    
    ArgCallback(void *raw, void *indirection);

    template<typename T>
    ArgCallback(T func_or_lambda);
    
    
    void operator()(Args ...data) const;
    operator bool() const;
};
#include "callback.inl"