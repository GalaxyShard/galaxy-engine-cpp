#include <Galaxy/event.hpp>

int Signal::connect_int(empty_func func)
{
    int id = nextID;
    listeners.insert(std::make_pair(id, func));
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
    
    for (auto pair : signal->listeners)
    {
        pair.second();
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
//EventListener::EventListener(std::shared_ptr<Signal> &isignal, empty_func func)
//{
//    signal = isignal;
//    eventID = isignal->connect(func);
//}
//void EventListener::disconnect()
//{
//    //if (auto sig = signal.lock()) sig->listeners.erase(eventID);
//    if (auto sig = signal.lock()) sig->disconnect(eventID);
//}