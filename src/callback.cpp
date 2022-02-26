#include <Galaxy/Events/callback.hpp>

Callback::Callback() : indirection(0) {}
void Callback::operator()()
{
    if (indirection)
        indirection(raw);
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