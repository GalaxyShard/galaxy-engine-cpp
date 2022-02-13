

template<typename T>
void print_overload(T &obj)
{
    std::cout << obj;
}
template<typename T>
void print_overload(std::vector<T> &obj)
{
    int size = obj.size()-1;
    for (int i = 0; i < size; ++i)
        std::cout << obj[i] << ", ";
    std::cout << obj[size+1];
}
template <unsigned int I, typename... Args>
inline std::enable_if_t<I == sizeof...(Args)> print_element(const std::string &format, std::tuple<Args...> &args, int index)
{ std::cout << format.substr(index); }
template <unsigned int I, typename... Args>
inline std::enable_if_t<I < sizeof...(Args)> print_element(const std::string &format, std::tuple<Args...> &args, int index)
{
    size_t start = format.find("%", index);
    if (format[start+1] == 'o')
    {
        std::cout << format.substr(index, start-index);
        print_overload(std::get<I>(args));
        print_element<I+1, Args...>(format, args, start+2);
    }
    else
    {
        size_t end = format.find("o", index);
        auto original = std::cout.precision();
        std::cout.precision(std::stoi(format.substr(start+2, end-start-2)));

        std::cout << format.substr(index, start-index);
        print_overload(std::get<I>(args));

        std::cout.precision(original);
        print_element<I+1, Args...>(format, args, end+1);
    }
}
/*
    Similar to printf, but use %o instead and %._o to print with precision _
    USAGE: print("Results: %.2o, %o\n", (10.1-1.21), 1-2);
    Prints "Results: 8.9, -1"
*/
template <typename... Args>
void print(const std::string &format, const Args& ...args)
{
    auto args_tuple = std::tuple<Args...>(args...);
    print_element<0, Args...>(format, args_tuple);
    std::cout.flush();
}