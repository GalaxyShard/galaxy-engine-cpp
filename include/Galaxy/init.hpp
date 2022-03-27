#pragma once

namespace Init
{
    typedef void(*init_func)();
    void fire();
    void fire_cleanup();

    void add(init_func);
    void add_first_init(init_func);
    void add_cleanup(init_func);
}
#define FIRST_INIT_FUNC(func) static char func##_first_init_ignore = (Init::add_first_init(&func),0)
#define INIT_FUNC(func) static char func##_init_ignore = (Init::add(&func),0)
#define CLEANUP_FUNC(func) static char func##_cleanup_ignore = (Init::add_cleanup(&func),0)
