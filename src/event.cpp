#include <Galaxy/Events/event.hpp>

int Signal::connect_int(Callback callback)
{
    int id = nextID;
    listeners.insert(std::make_pair(id, callback));
    ++nextID;
    return id;
}
std::unique_ptr<Listener> Signal::connect(Callback callback)
{
    return std::make_unique<Listener>(this, connect_int(callback));
}
void Signal::disconnect_int(int id)
{
    eraseQueue.push_back(id);
}
void Event::fire()
{
    for (auto id : signal.eraseQueue)
        signal.listeners.erase(id);
    signal.eraseQueue.clear();
    
    for (auto &[id, listener] : signal.listeners)
        listener();
    
    //for (auto id : signal->eraseQueue) { signal->listeners.erase(id); }
    //signal->eraseQueue.clear();
    
    //for (auto &&[id, listener] : signal->listeners)
    //{
    //    listener();
    //}
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