#include <Galaxy/Events/callback.hpp>

Callback::Callback() : indirection(0) {}
Callback::Callback(std::nullptr_t) : indirection(0) {}
void Callback::operator()() const
{
    if (indirection)
        indirection(raw);
}
Callback::operator bool() const
{
    return indirection;
}
Callback::Callback(void *raw, void *indirection)
    : raw(raw), indirection((void(*)(void*))indirection) {}
RawCallback::RawCallback(Callback c)
    : data(c.raw), indirection((void*)c.indirection) {}
