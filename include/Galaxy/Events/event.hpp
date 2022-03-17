#pragma once
#include <unordered_map>
#include <vector>
#include <Galaxy/Events/callback.hpp>
struct Listener;
class Signal final
{
private:
    std::unordered_map<int, Callback> listeners;
    std::vector<int> eraseQueue;
    int nextID = 0;

    friend class Event;
public:
    int connect_int(Callback callback);
    void disconnect_int(int id);
    std::unique_ptr<Listener> connect(Callback callback);
};
class Event final
{
public:
    // possibly use no pointer
    std::unique_ptr<Signal> signal = std::make_unique<Signal>();
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
struct ListenerT;


template<typename T>
class SignalT final
{
private:
    int nextID = 0;
    std::unordered_map<int, ArgCallback<T>> listeners;
    std::vector<int> eraseQueue;

    friend class EventT<T>;
public:
    int connect_int(ArgCallback<T> callback);
    void disconnect_int(int id);
    std::unique_ptr<ListenerT<T>> connect(ArgCallback<T> callback);
};
template<typename T>
class EventT final
{
public:
    // possibly use no pointer
    std::unique_ptr<SignalT<T>> signal = std::make_unique<SignalT<T>>();
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
#include "event.inl"