
template<typename T>
ListenerT<T> SignalT<T>::connect(ArgCallback<T> callback)
{
    return ListenerT<T>(this, connect_int(callback));
}
template<typename T>
int SignalT<T>::connect_int(ArgCallback<T> callback)
{
    int id = nextID;
    listeners.insert(std::make_pair(id, callback));
    ++nextID;
    return id;
}
template<typename T>
void SignalT<T>::disconnect_int(int id)
{
    eraseQueue.push_back(id);
}
template <typename T>
void EventT<T>::fire(T data)
{
    for (auto id : signal.eraseQueue)
        signal.listeners.erase(id);
    signal.eraseQueue.clear();

    for (auto &&[id, listener] : signal.listeners)
        listener(data);
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
ListenerT<T>::ListenerT(ListenerT &&v) : signal(std::move(v.signal)), id(std::move(v.id))
{
    v.signal = 0;
    v.id = -1;
}
template <typename T>
ListenerT<T>& ListenerT<T>::operator=(ListenerT &&v)
{
    if (&v != this)
    {
        signal = v.signal;
        id = v.id;
        v.signal = 0;
        v.id = -1;
    }
    return *this;
}

template <typename T>
ListenerT<T>::~ListenerT()
{
    disconnect();
}