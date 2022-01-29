
template<typename T>
int SignalT<T>::connect_int(SignalT<T>::signal_func func)
{
    int id = nextID;
    listeners.insert(std::make_pair(id, SignalFunc{.voidFunc=func}));
    ++nextID;
    return id;
}
template <typename T>
template <typename U>
int SignalT<T>::connect_int(U *inst, void(U::*func)(T data))
{
    int id = nextID;
    typedef void(U::*member_func)(T data);
    listeners.insert(std::make_pair(id, SignalFunc{
        .classFunc=reinterpret_cast<signal_member>(func),
        .memberLambda=[](void *inst, signal_member classFunc, T data)
        { (((U*)inst)->*(reinterpret_cast<member_func>(classFunc)))(data); },
        .inst=inst
    }));
    ++nextID;
    return id;
}
template <typename T>
void SignalT<T>::disconnect_int(int id)
{
    eraseQueue.push_back(id);
}
template <typename T>
std::unique_ptr<ListenerT<T>> SignalT<T>::connect(signal_func func)
{
    return std::make_unique<ListenerT<T>>(this, connect_int(func));
}
template <typename T>
template <typename U>
std::unique_ptr<ListenerT<T>> SignalT<T>::connect(U *inst, void(U::*func)(T data))
{
    return std::make_unique<ListenerT<T>>(this, connect_int(inst, func));
}
template <typename T>
void EventT<T>::fire(T data) const
{
    for (auto id : signal->eraseQueue) { signal->listeners.erase(id); }
    signal->eraseQueue.clear();
    for (auto &&[id, listener] : signal->listeners)
    {
        if (listener.inst) listener.memberLambda(listener.inst, listener.classFunc, data);
        else listener.voidFunc(data);
    }
}

template <typename T>
void ListenerT<T>::disconnect()
{
    if (id != -1)
    {
        signal->disconnect_int(id);
        id = -1;
    }
}
template <typename T>
ListenerT<T>::ListenerT() { }
template <typename T>
ListenerT<T>::ListenerT(SignalT<T> *sig, int id) : signal(sig), id(id) { }
template <typename T>
ListenerT<T>::~ListenerT()
{
    disconnect();
}