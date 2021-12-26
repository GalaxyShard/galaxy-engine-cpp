#pragma once
#include <unordered_map>
#include <vector>
typedef void (*empty_func)();
struct Listener;
class Signal final
{
    private:
        int nextID = 0;
        std::unordered_map<int, empty_func> listeners;
        std::vector<int> eraseQueue;

        friend class Event;
    public:
        int connect_int(empty_func func);
        void disconnect_int(int id);
        std::unique_ptr<Listener> connect(empty_func func);
        //Listener connect(empty_func func);
};
class Event final
{
    public:
        // possibly use raw pointer, all references to this are weak so there is no need for counting
        std::shared_ptr<Signal> signal = std::make_shared<Signal>();
        void fire() const;
};
struct Listener
{
    Signal *signal;
    int id = -1;

    void disconnect();

    Listener();
    Listener(Signal *sig, int id);
    ~Listener();

    Listener(const Listener&) = delete;
    void operator=(const Listener&) = delete;
};



template<typename T>
class EventT;

template<typename T>
class ListenerT;

template<typename T>
class SignalT final
{
    private:
        typedef void (*signal_func)(T data);
        int nextID = 0;
        std::unordered_map<int, signal_func> listeners;
        std::vector<int> eraseQueue;

        friend class EventT<T>;
    public:
        int connect_int(signal_func func);
        void disconnect_int(int id);
        std::unique_ptr<ListenerT<T>> connect(signal_func func);
};
template<typename T>
class EventT final
{
    public:
        // possibly use raw pointer, all references to this are weak so there is no need for counting
        std::shared_ptr<SignalT<T>> signal = std::make_shared<SignalT<T>>();
        void fire(T data) const;
};
template<typename T>
struct ListenerT
{
    SignalT<T> *signal;
    int id = -1;

    void disconnect();

    ListenerT();
    ListenerT(SignalT<T> *sig, int id);
    ~ListenerT();

    ListenerT(const ListenerT&) = delete;
    void operator=(const ListenerT&) = delete;
};
template<typename T>
int SignalT<T>::connect_int(SignalT<T>::signal_func func)
{
    int id = nextID;
    listeners.insert(std::make_pair(id, func));
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
void EventT<T>::fire(T data) const
{
    for (auto id : signal->eraseQueue) { signal->listeners.erase(id); }
    signal->eraseQueue.clear();
    
    for (auto pair : signal->listeners)
    {
        pair.second(data);
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