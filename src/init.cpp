#include <internalinit.hpp>
#include <queue>
#include <stack>
using Init::init_func;
//static std::queue<init_func> *internalQueue, *firstInitQueue, *initQueue, *cleanupQueue;
static std::vector<init_func> *internalQueue, *firstInitQueue, *initQueue, *cleanupQueue;
//static inline void use_queue(std::queue<init_func> *q)
static inline void use_queue(std::vector<init_func> *q)
{
    if (q == nullptr) return;
    //while(!q->empty())
    //{
    //    q->front()();
    //    q->pop();
    //}
    for (auto &func : *q)
        func();
    
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
//static inline void* add_func(std::queue<init_func> *&q, init_func func)
static inline char add_func(std::vector<init_func> *&q, init_func func)
{
    //if (q == nullptr) q = new std::queue<init_func>();
    //q->push(func);
    if (q == nullptr) q = new std::vector<init_func>();
    q->push_back(func);
    return 0;
}
char Init::add_internal(init_func func) { return add_func(internalQueue, func); }
char Init::add_first_init(init_func func) { return add_func(firstInitQueue, func); }
char Init::add(init_func func){ return add_func(initQueue, func); }
char Init::add_cleanup(init_func func) { return add_func(cleanupQueue, func); }
