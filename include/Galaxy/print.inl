

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
    std::cout << obj[size];
}
template <unsigned int I, typename... Args>
inline std::enable_if_t<I == sizeof...(Args)> print_element(const std::string &format, std::tuple<Args...> &args, int index)
{ std::cout << format.substr(index); }

template <unsigned int I, typename... Args>
inline std::enable_if_t<I < sizeof...(Args)> print_element(const std::string &format, std::tuple<Args...> &args, int index)
{
    auto start = format.find("%", index);
    auto end = format.find("o", start);

    auto original = std::cout.precision();
    if (end != start+1) std::cout.precision(std::stoi(format.substr(start+2, end-start-2)));;

    std::cout << format.substr(index, start-index);
    print_overload(std::get<I>(args));

    std::cout.precision(original);
    print_element<I+1, Args...>(format, args, end+1);
}

template <typename... Args>
void print(const std::string &format, const Args& ...args)
{
    auto args_tuple = std::tuple<Args...>(args...);
    print_element<0, Args...>(format, args_tuple);
    std::cout.flush();
}