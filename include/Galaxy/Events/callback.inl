#include <iostream>
template<typename T>
Callback::Callback(T func_or_lambda)
{
    static_assert(std::is_invocable<T>::value, "Not a function or a lambda");
    static_assert(sizeof(T) <= 8, "Lambda too large");
    memcpy(raw, &func_or_lambda, sizeof(T));
    indirection = [](char *buffer) { (*(decltype(func_or_lambda)*)buffer)(); };
}
//template<typename T>
//ArgCallback<T>::operator bool() const
template<typename... Args>
ArgCallback<Args...>::operator bool() const
{
    return indirection;
}

//template<typename T>
//ArgCallback<T>::ArgCallback() : indirection(0) {}
template<typename... Args>
ArgCallback<Args...>::ArgCallback() : indirection(0) {}
//template<typename T>
//ArgCallback<T>::ArgCallback(std::nullptr_t) : indirection(0) {}
template<typename... Args>
ArgCallback<Args...>::ArgCallback(std::nullptr_t) : indirection(0) {}

//template<typename T>
//ArgCallback<T>::ArgCallback(U func_or_lambda)
template<typename... Args>
template<typename U>
ArgCallback<Args...>::ArgCallback(U func_or_lambda)
{
    //static_assert(std::is_invocable<U, T>::value, "Not a function or a lambda");
    static_assert(std::is_invocable<U, Args...>::value, "Not a function or a lambda");
    static_assert(sizeof(U) <= 8, "Lambda too large");
    memcpy(raw, &func_or_lambda, sizeof(U));
    //indirection = [](char *buffer, T data) { (*(decltype(func_or_lambda)*)buffer)(data); };
    indirection = [](char *buffer, Args ...data) { (*(decltype(func_or_lambda)*)buffer)(data...); };
}
template<typename... Args>
void ArgCallback<Args...>::operator()(Args ...data)
{
    if (indirection)
        indirection(raw, data...);
}
//template<typename T>
//void ArgCallback<T>::operator()(T data)
//{
//    if (indirection)
//        indirection(raw, data);
//}