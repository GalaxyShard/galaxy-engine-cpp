#include <iostream>
template<typename T>
Callback::Callback(T func_or_lambda)
{
    static_assert(std::is_invocable<T>::value, "Not a function or a lambda");
    static_assert(sizeof(T) <= 8, "Lambda too large");
    memcpy(raw, &func_or_lambda, sizeof(T));
    indirection = [](char *buffer) { (*(decltype(func_or_lambda)*)buffer)(); };
}

template<typename T>
ArgCallback<T>::ArgCallback() : indirection(0) {}
template<typename T>
template<typename U>
ArgCallback<T>::ArgCallback(U func_or_lambda)
{
    static_assert(std::is_invocable<U, T>::value, "Not a function or a lambda");
    static_assert(sizeof(U) <= 8, "Lambda too large");
    memcpy(raw, &func_or_lambda, sizeof(U));
    indirection = [](char *buffer, T data) { (*(decltype(func_or_lambda)*)buffer)(data); };
}
template<typename T>
void ArgCallback<T>::operator()(T data)
{
    if (indirection)
        indirection(raw, data);
}



//template<typename T>
//Callback::Callback(T *inst, void (T::*func)())
//{
//    typedef void(T::*t_member)();
//    raw.classFunc = reinterpret_cast<RawCallback::member>(func);
//    raw.inst = inst;
//    raw.memberLambda = [](void *inst, RawCallback::member classFunc)
//    { (((T*)inst)->*(reinterpret_cast<t_member>(classFunc)))(); };
//}


//template<typename U>
//ArgCallback<U>::ArgCallback()
//{
//    raw.voidFunc = 0;
//}
//template<typename U>
//ArgCallback<U>::ArgCallback(void (*func)(U))
//{
//    raw.voidFunc = func;
//}
//template<typename U>
//template<typename T>
//ArgCallback<U>::ArgCallback(T *inst, void (T::*func)(U))
//{
//    typedef void(T::*t_member)(U);
//    raw.classFunc = reinterpret_cast<raw_member>(func);
//    raw.inst = inst;
//    raw.memberLambda = [](void *inst, raw_member classFunc, U arg)
//        { (((T*)inst)->*(reinterpret_cast<t_member>(classFunc)))(arg); };
//}

//template<typename U>
//void ArgCallback<U>::operator()(U arg)
//{
//    if (raw.inst) raw.memberLambda(raw.inst, raw.classFunc, arg);
//    else if (raw.voidFunc) raw.voidFunc(arg);
//}