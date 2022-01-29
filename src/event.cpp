#include <Galaxy/event.hpp>

int Signal::connect_int(empty_func func)
{
    int id = nextID;
    listeners.insert(std::make_pair(id, EmptyFunc{.capture=std::unique_ptr<CaptureBase>((CaptureBase*)new NoCapture(func))}));
    //listeners.insert(std::make_pair(id, func));
    ++nextID;
    return id;
}
std::unique_ptr<Listener> Signal::connect(empty_func func)
{
    return std::make_unique<Listener>(this, connect_int(func));
}
void Signal::disconnect_int(int id)
{
    eraseQueue.push_back(id);
}
void Event::fire() const
{
    for (auto id : signal->eraseQueue) { signal->listeners.erase(id); }
    signal->eraseQueue.clear();
    
    for (auto &&[id, listener] : signal->listeners)
    {
        if (listener.inst) listener.memberLambda(listener.inst, listener.capture.get());
        else ((Signal::NoCapture*)listener.capture.get())->func();
    }
}
Listener::Listener() { }
Listener::Listener(Signal *sig, int id) : signal(sig), id(id) { }
Listener::~Listener()
{
    disconnect();
}
void Listener::disconnect()
{
    if (id != -1)
    {
        signal->disconnect_int(id);
        id = -1;
    }
}