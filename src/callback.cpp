#include <Galaxy/callback.hpp>

Callback::Callback(void (*func)())
{
    raw.voidFunc = func;
}
void Callback::operator()()
{
    if (raw.inst) raw.memberLambda(raw.inst, raw.classFunc);
    else raw.voidFunc();
}