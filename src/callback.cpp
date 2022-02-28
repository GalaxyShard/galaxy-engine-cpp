#include <Galaxy/Events/callback.hpp>

Callback::Callback() : indirection(0) {}
Callback::Callback(std::nullptr_t) : indirection(0) {}
void Callback::operator()()
{
    if (indirection)
        indirection(raw);
}
Callback::operator bool() const
{
    return indirection;
}
//void test()
//{
//    NewCallback(1);
//    NewCallback([](){});
//}
//Callback::Callback()
//{
//    raw.voidFunc = 0;
//}
//Callback::Callback(void (*func)())
//{
//    raw.voidFunc = func;
//}
//void Callback::operator()()
//{
//    if (raw.inst) raw.memberLambda(raw.inst, raw.classFunc);
//    else if (raw.voidFunc) raw.voidFunc();
//}