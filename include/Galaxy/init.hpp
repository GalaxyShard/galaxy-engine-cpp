#pragma once
//#include <queue>

namespace Init
{
    typedef void(*init_func)();
    void fire();
    void fire_cleanup();

    void* add(init_func);
    //void* add_internal(init_func);
    void* add_first_init(init_func);
    void* add_cleanup(init_func func);
}
//#define INTERNAL_INIT_FUNC(func) static auto _internal_init_ignore = Init::add_internal(&func);
#define FIRST_INIT_FUNC(func) static auto _first_init_ignore = Init::add_first_init(&func);
#define INIT_FUNC(func) static auto _init_ignore = Init::add(&func);
#define CLEANUP_FUNC(func) static auto _cleanup_ignore = Init::add_cleanup(&func);
