#include <Galaxy/event.hpp>

//void Signal::FastVectorInt::push_back(int element)
//{
//    if (_size == _capacity)
//    {
//        _capacity *= 2;
//        int *newArray = new int[_capacity];
//        memcpy(newArray, array, _size);
//        delete[] array;
//        array = newArray;
//    }
//    array[_size] = element;
//    ++_size;
//}
//unsigned char Signal::FastVectorInt::size()
//{ return _size; }
//void Signal::FastVectorInt::clear()
//{ _size = 0; }
//int &Signal::FastVectorInt::operator[](int index)
//{
//    return array[index];
//}
//Signal::FastVectorInt::FastVectorInt()
//{
//    array = new int[_capacity];
//}
//Signal::FastVectorInt::~FastVectorInt()
//{
//    delete[] array;
//}


int Signal::connect_int(empty_func func)
{
    int id = nextID;
    //listeners.insert(std::make_pair(id, EmptyFunc{.voidFunc = func}));
    listeners.insert(std::make_pair(id, Callback(func)));
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
    //for (int i = 0; i < signal->eraseQueue.size(); ++i) { signal->listeners.erase(signal->eraseQueue[i]); }
    signal->eraseQueue.clear();
    
    for (auto &&[id, listener] : signal->listeners)
    {
        listener();
        //if (listener.inst) listener.memberLambda(listener.inst, listener.classFunc);
        //else listener.voidFunc();
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