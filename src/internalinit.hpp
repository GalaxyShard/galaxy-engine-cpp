#include <Galaxy/init.hpp>

namespace Init
{
    void add_internal(init_func);
}
#define INTERNAL_INIT_FUNC(func) static char func##_internal_init_ignore = (Init::add_internal(&func),0)
