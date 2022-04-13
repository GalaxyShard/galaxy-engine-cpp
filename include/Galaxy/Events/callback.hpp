#pragma once

#define CLASS_LAMBDA(func) [this](){func();}
#define TYPE_LAMBDA(func, T) [this](T v){func(v);}

class Callback;
template<typename... Args>
class ArgCallback;
struct RawCallback
{
    void *data;
    void *indirection;
    //explicit operator Callback() const;
    //template<typename... Args>
    //explicit operator ArgCallback<Args...>() const;

    explicit RawCallback(Callback c);
    template<typename... Args>
    explicit RawCallback(ArgCallback<Args...> c);

    //RawCallback(void *data, void *indirection)
    //    : data(data), indirection(indirection) { }
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