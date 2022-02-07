
template<typename T>
Callback::Callback(T *inst, void (T::*func)())
{
    typedef void(T::*t_member)();
    raw.classFunc = reinterpret_cast<RawCallback::member>(func);
    raw.inst = inst;
    raw.memberLambda = [](void *inst, RawCallback::member classFunc)
    { (((T*)inst)->*(reinterpret_cast<t_member>(classFunc)))(); };
}


template<typename U>
ArgCallback<U>::ArgCallback()
{
    raw.voidFunc = 0;
}
template<typename U>
ArgCallback<U>::ArgCallback(void (*func)(U))
{
    raw.voidFunc = func;
}
template<typename U>
template<typename T>
ArgCallback<U>::ArgCallback(T *inst, void (T::*func)(U))
{
    typedef void(T::*t_member)(U);
    raw.classFunc = reinterpret_cast<raw_member>(func);
    raw.inst = inst;
    raw.memberLambda = [](void *inst, raw_member classFunc, U arg)
        { (((T*)inst)->*(reinterpret_cast<t_member>(classFunc)))(arg); };
}

template<typename U>
void ArgCallback<U>::operator()(U arg)
{
    if (raw.inst) raw.memberLambda(raw.inst, raw.classFunc, arg);
    else if (raw.voidFunc) raw.voidFunc(arg);
}