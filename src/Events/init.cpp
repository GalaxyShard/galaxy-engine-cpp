#include <internalinit.hpp>
#include <queue>
#include <stack>

// Temporary fix because static variabless arent initialized unless a function is called in that file
// (audio/input is not properly initialized)
// Possibly use DLLs, a map of function pointers, or a engineMain function
extern void iinit_input();
extern void iinit_renderer();
extern void iinit_shader();
extern void iinit_audio();
extern void init_image();
extern void init_text();
extern void cleanup_audio();
extern void cleanup_net();

using Init::init_func;
static std::vector<init_func> *startQueues=0, *cleanupQueue=0;

static inline void use_queue(std::vector<init_func> *q)
{
    for (auto &func : *q)
        func();
}
void Init::fire()
{
    iinit_input();
    iinit_renderer();
    iinit_shader();
    iinit_audio();
    init_image();
    init_text();

    if (!startQueues) return;
    use_queue(startQueues);
    use_queue(startQueues+1);
    use_queue(startQueues+2);
    delete[] startQueues;
}
void Init::fire_cleanup()
{
    cleanup_audio();
    cleanup_net();
    if (!cleanupQueue) return;
    use_queue(cleanupQueue);
    delete cleanupQueue;
}
static inline std::vector<init_func> *start_queues()
{
    if (!startQueues) startQueues = new std::vector<init_func>[3];
    return startQueues;
}
void Init::add_internal(init_func func)
{
    start_queues()->push_back(func);
}
void Init::add_first_init(init_func func)
{
    start_queues()[1].push_back(func);
}
void Init::add(init_func func)
{
    start_queues()[2].push_back(func);
}
void Init::add_cleanup(init_func func)
{
    if (!cleanupQueue) cleanupQueue = new std::vector<init_func>();
    cleanupQueue->push_back(func);
}