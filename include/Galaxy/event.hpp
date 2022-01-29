#pragma once
#include <unordered_map>
#include <vector>
typedef void (*empty_func)();
struct Listener;
class Signal final
{
private:
    class CaptureBase {  };
    class NoCapture : public CaptureBase
    {
    public:
        void (*func)();
        NoCapture(void (*func)()) : func(func) {  }
    };
    template<typename T>
    class CaptureT : public CaptureBase
    {
    public:
        void (T::*func)();
        CaptureT(void (T::*func)()) : func(func) {  }
    };
    struct EmptyFunc
    {
        void *inst=0;
        std::unique_ptr<CaptureBase> capture;
        void(*memberLambda)(void *inst, CaptureBase *member);
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
            .capture = std::unique_ptr<CaptureBase>((CaptureBase*)new CaptureT<T>(func)),
            .memberLambda=[](void *inst, CaptureBase *capture) { (((T*)inst)->*(((CaptureT<T>*)capture)->func))(); },
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
    class CaptureBase {  };
    class NoCapture : public CaptureBase
    {
    public:
        void (*func)(T data);
        NoCapture(void (*func)(T data)) : func(func) {  }
    };
    template<typename U>
    class CaptureT : public CaptureBase
    {
    public:
        void (U::*func)(T data);
        CaptureT(void (U::*func)(T data)) : func(func) {  }
    };
    struct SignalFunc
    {
        void *inst=0;
        std::unique_ptr<CaptureBase> capture;
        void(*memberLambda)(void *inst, CaptureBase *member, T data);
    };
//private:
    typedef void (*signal_func)(T data);
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