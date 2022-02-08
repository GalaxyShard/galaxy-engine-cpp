#include <Galaxy/callback.hpp>

Callback::Callback()
{
    raw.voidFunc = 0;
}
Callback::Callback(void (*func)())
{
    raw.voidFunc = func;
}
void Callback::operator()()
{
    if (raw.inst) raw.memberLambda(raw.inst, raw.classFunc);
    else if (raw.voidFunc) raw.voidFunc();
}