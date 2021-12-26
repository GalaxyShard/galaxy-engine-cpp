//#include <Galaxy/init.hpp>
#include <internalinit.hpp>

//#include <iostream>
#include <queue>
using Init::init_func;
//static std::queue<init_func> *initQueue, *internalQueue, *cleanupQueue;
static std::queue<init_func> *internalQueue, *firstInitQueue, *initQueue, *cleanupQueue;
static inline void use_queue(std::queue<init_func> *q)
{
    if (q == nullptr) return;
    while(!q->empty())
    {
        q->front()();
        q->pop();
    }
    delete q;
}
void Init::fire()
{
    use_queue(internalQueue);
    use_queue(firstInitQueue);
    use_queue(initQueue);
}
void Init::fire_cleanup()
{
    use_queue(cleanupQueue);
}
static inline void* add_func(std::queue<init_func> *&q, init_func func)
{
    if (q == nullptr) q = new std::queue<init_func>();
    q->push(func);
    return nullptr;
}
void* Init::add_internal(init_func func) { return add_func(internalQueue, func); }
void* Init::add_first_init(init_func func) { return add_func(firstInitQueue, func); }
void* Init::add(init_func func){ return add_func(initQueue, func); }
void* Init::add_cleanup(init_func func) { return add_func(cleanupQueue, func); }

//#define QUEUE_FUNC(q) if (q == nullptr) q = new std::queue<init_func>(); \
//    q->push(func); \
//    return nullptr; 

//void* Init::add(init_func func) { QUEUE_FUNC(initQueue); }
//void* Init::add_internal(init_func func) { QUEUE_FUNC(internalQueue); }
//void* Init::add_cleanup(init_func func) { QUEUE_FUNC(cleanupQueue) }