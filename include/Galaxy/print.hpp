#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
namespace Log
{
    template<typename T>
    void print_overload(std::ostream &stream, T &obj);
    template<typename T>
    void print_overload(std::ostream &stream, std::vector<T> &obj);
    template <unsigned int I = 0, typename... Args>
    inline std::enable_if_t<I == sizeof...(Args)> print_element(std::ostream &stream, const std::string &format, std::tuple<Args...> &args, int index = 0);
    template <unsigned int I = 0, typename... Args>
    inline std::enable_if_t<I < sizeof...(Args)> print_element(std::ostream &stream, const std::string &format, std::tuple<Args...> &args, int index = 0);

    template <typename... Args>
    void print(std::ostream &stream, const std::string &format, const Args& ...args);
    template <typename T, typename... Args>
    void print(std::ostream &stream, const std::string &format, const T &arg0, const Args& ...args);
}
/*
    Similar to printf, but use %o for everything and %._o to print with precision _
    USAGE: logmsg("Results: %.2o, %o\n", (10.1-1.21), 1-2);
    Prints "Results: 8.9, -1"
*/
template <typename... Args>
void logmsg(const std::string &format, const Args& ...args);
template <typename... Args>
void logerr(const std::string &format, const Args& ...args);

#include "print.inl"