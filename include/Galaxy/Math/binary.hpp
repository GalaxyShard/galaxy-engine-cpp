#pragma once
#include <string>

template<typename T>
T b_cast(const void *buffer)
{
    T data;
    memcpy(&data, buffer, sizeof(T));
    return data;
}

class BinaryReader
{
private:
    std::string buffer;
    int nextIndex = 0;
public:
    // use full template specialization
    // users should be able to add their own specializations
    // all the standard types should be built in (char,float,int,vectors)
    template<typename T>
    T read();

    std::string& get_buffer() { return buffer; }
    const std::string& get_buffer() const { return buffer; }

    BinaryReader(std::string buffer) : buffer(buffer) {}
};
class BinaryWriter
{
private:
    std::string buffer;
public:
    // use full template specialization
    // users should be able to add their own specializations
    // all the standard types should be built in (char,float,int,vectors)
    template<typename T>
    void write(T);

    std::string& get_buffer() { return buffer; }
    const std::string& get_buffer() const { return buffer; }
};