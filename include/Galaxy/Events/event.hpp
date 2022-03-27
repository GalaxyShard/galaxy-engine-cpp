#pragma once
#include <map>
#include <vector>
#include <Galaxy/Events/callback.hpp>
struct Listener;
class Signal final
{
private:
    std::map<int, Callback> listeners;
    std::vector<int> eraseQueue;
    int nextID = 0;

    friend class Event;
public:
    int connect_int(Callback callback);
    void disconnect_int(int id);
    Listener connect(Callback callback);
};
class Event final
{
public:
    Signal signal;
    void fire();
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
    Listener& operator=(const Listener&) = delete;
    Listener(Listener&&);
    Listener& operator=(Listener&&);
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
    std::map<int, ArgCallback<T>> listeners;
    std::vector<int> eraseQueue;

    friend class EventT<T>;
public:
    int connect_int(ArgCallback<T> callback);
    void disconnect_int(int id);
    ListenerT<T> connect(ArgCallback<T> callback);
};
template<typename T>
class EventT final
{
public:
    SignalT<T> signal;
    void fire(T data);
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
    ListenerT& operator=(const ListenerT&) = delete;
    ListenerT(ListenerT&&);
    ListenerT& operator=(ListenerT&&);
};
#include "event.inl"