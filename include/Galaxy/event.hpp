#pragma once
#include <unordered_map>
#include <vector>
struct Listener;
class EmptyClass{};

class Signal final
{
private:
    typedef void (*empty_func)();
    typedef void (EmptyClass::*empty_member)();
    struct EmptyFunc
    {
        void *inst=0;
        union
        {
            empty_func voidFunc;
            struct
            {
                empty_member classFunc;
                void (*memberLambda)(void *inst, empty_member classFunc);
            };
        };
    };
    std::unordered_map<int, EmptyFunc> listeners;
    std::vector<int> eraseQueue;
    int nextID = 0;

    friend class Event;
public:
    int connect_int(empty_func func);
    template <typename T>
    int connect_int(T *inst, void(T::*func)())
    {
        int id = nextID;
        typedef void(T::*member_func)();
        listeners.insert(std::make_pair(id, EmptyFunc{
            .classFunc = reinterpret_cast<empty_member>(func),
            .memberLambda=[](void *inst, empty_member classFunc)
            { (((T*)inst)->*(reinterpret_cast<member_func>(classFunc)))(); },

            .inst=inst
        }));
        ++nextID;
        return id;
    }
    void disconnect_int(int id);
    std::unique_ptr<Listener> connect(empty_func func);

    template <typename T>
    std::unique_ptr<Listener> connect(T *inst, void(T::*func)())
    {
        return std::make_unique<Listener>(this, connect_int(inst, func));
    }
};
class Event final
{
public:
    // possibly use unique pointer, all references are raw so no need for ref counting
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
    typedef void (EmptyClass::*signal_member)(T data);
    struct SignalFunc
    {
        void *inst=0;
        union
        {
            signal_func voidFunc;
            struct
            {
                signal_member classFunc;
                void (*memberLambda)(void *inst, signal_member classFunc, T data);
            };
        };
    };
    int nextID = 0;
    std::unordered_map<int, SignalFunc> listeners;
    std::vector<int> eraseQueue;

    friend class EventT<T>;
public:
    int connect_int(signal_func func);
    template <typename U>
    int connect_int(U *inst, void(U::*func)(T data));

    void disconnect_int(int id);
    std::unique_ptr<ListenerT<T>> connect(signal_func func);

    template <typename U>
    std::unique_ptr<ListenerT<T>> connect(U *inst, void(U::*func)(T data));
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
#include "event.inl"