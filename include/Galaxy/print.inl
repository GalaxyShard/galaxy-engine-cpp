

template<typename T>
void Debug::print_overload(std::ostream &stream, T &obj)
{
    stream << obj;
}
template<typename T>
void Debug::print_overload(std::ostream &stream, std::vector<T> &obj)
{
    int size = obj.size()-1;
    for (int i = 0; i < size; ++i)
        stream << obj[i] << ", ";
    stream << obj[size];
}
template <unsigned int I, typename... Args>
inline std::enable_if_t<I == sizeof...(Args)> Debug::print_element(std::ostream &stream, const std::string &format, std::tuple<Args...> &args, int index)
{ stream << format.substr(index); }

template <unsigned int I, typename... Args>
inline std::enable_if_t<I < sizeof...(Args)> Debug::print_element(std::ostream &stream, const std::string &format, std::tuple<Args...> &args, int index)
{
    auto start = format.find("%", index);
    auto end = format.find("o", start);

    auto original = stream.precision();
    if (end != start+1) std::cout.precision(std::stoi(format.substr(start+2, end-start-2)));;

    stream << format.substr(index, start-index);
    print_overload(stream, std::get<I>(args));

    stream.precision(original);
    print_element<I+1, Args...>(stream, format, args, end+1);
}

template <typename... Args>
void Debug::print(std::ostream &stream, const std::string &format, const Args& ...args)
{
    auto args_tuple = std::tuple<Args...>(args...);
    print_element<0, Args...>(stream, format, args_tuple);
    stream.flush();
}
template <typename... Args>
void Debug::log(const std::string &format, const Args& ...args)
{
    print(std::cout, format, args...);
}
template <typename... Args>
void Debug::logerror(const std::string &format, const Args& ...args)
{
    print(std::cerr, format, args...);
}