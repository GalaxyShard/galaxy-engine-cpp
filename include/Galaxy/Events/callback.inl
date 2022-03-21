#include <iostream>
template<typename T>
Callback::Callback(T func_or_lambda)
{
    static_assert(std::is_invocable<T>::value, "Not a function or a lambda");
    static_assert(sizeof(T) <= sizeof(raw), "Lambda too large");

    memcpy(&raw, &func_or_lambda, sizeof(T));
    indirection = [](void *buffer) { (*(T*)&buffer)(); };
}
template<typename... Args>
ArgCallback<Args...>::operator bool() const
{
    return indirection;
}
template<typename... Args>
ArgCallback<Args...>::ArgCallback() : indirection(0) {}
template<typename... Args>
ArgCallback<Args...>::ArgCallback(std::nullptr_t) : indirection(0) {}

template<typename... Args>
template<typename T>
ArgCallback<Args...>::ArgCallback(T func_or_lambda)
{
    static_assert(std::is_invocable<T, Args...>::value, "Not a function or a lambda");
    static_assert(sizeof(T) <= sizeof(raw), "Lambda too large");
    
    memcpy(&raw, &func_or_lambda, sizeof(T));
    indirection = [](void *buffer, Args ...data) { (*(T*)&buffer)(data...); };
}
template<typename... Args>
void ArgCallback<Args...>::operator()(Args ...data)
{
    if (indirection)
        indirection(raw, data...);
}