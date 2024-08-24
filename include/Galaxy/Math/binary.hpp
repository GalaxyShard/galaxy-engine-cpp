#pragma once
#include <string>
#include <fstream>
#include <cassert>
#include <cstdint>
#include <cstring>
enum Endian:unsigned char { LITTLE, BIG };

Endian sys_endian();
template<typename T>
T b_cast(const void *buffer)
{
    T data;
    memcpy(&data, buffer, sizeof(T));
    return data;
}
template<typename T>
T set_endian32(char *buffer, Endian from, Endian to)
{
    static_assert(sizeof(T)==4, "non-32 bit type");
    if (from != to)
    {
        std::swap(buffer[0], buffer[3]);
        std::swap(buffer[1], buffer[2]);
    }
    return b_cast<T>(buffer);
}
template<typename T>
T set_endian16(char *buffer, Endian from, Endian to)
{
    static_assert(sizeof(T)==2, "non-16 bit type");
    if (from != to)
        std::swap(buffer[0], buffer[1]);
    return b_cast<T>(buffer);
}
template<typename T>
T to_native_endian32(char *buffer, Endian current) { return set_endian32<T>(buffer, current, sys_endian()); }
template<typename T>
T to_native_endian16(char *buffer, Endian current) { return set_endian16<T>(buffer, current, sys_endian()); }

template<typename T>
T set_endian(char *buffer, Endian from, Endian to)
{
    if constexpr(sizeof(T)==4)
        return set_endian32<T>(buffer,from,to);
    else if constexpr(sizeof(T)==2)
        return set_endian16<T>(buffer,from,to);
    else if constexpr(sizeof(T)==1)
        return b_cast<T>(buffer);
    else assert(false && "set_endian only works for 32, 16, and 8 bit types");
}
template<typename T>
T to_native_endian(char *buffer, Endian current)
{
    if constexpr(sizeof(T)==4)
        return to_native_endian32<T>(buffer,current);
    else if constexpr(sizeof(T)==2)
        return to_native_endian16<T>(buffer,current);
    else if constexpr(sizeof(T)==1)
        return b_cast<T>(buffer);
    else assert(false && "to_native_endian only works for 32, 16, and 8 bit types");
}

struct BinaryBuffer
{
    typedef bool(*read_bytes_func)(void *reader, char* buffer, uint32_t num);
    void *reader;
    read_bytes_func readBytes; // reads bytes to buffer
    char *temp; // 8 byte char array
    BinaryBuffer(void *reader, read_bytes_func readBytes, char *temp)
        : reader(reader), readBytes(readBytes), temp(temp) {}

    bool read(char* buffer, uint32_t num) { return readBytes(reader,buffer,num); }
};
// use full template specialization
// users should be able to add their own specializations
// all the standard types should be built in (char,float,int,vectors)
template<typename T>
T deserialize(BinaryBuffer &buffer, Endian endian = BIG);

class BinaryFileReader
{
public:
    std::ifstream stream;
    char bytes[8];
    Endian endian;
    BinaryBuffer buffer;
    template<typename T>
    T read()
    {
        return deserialize<T>(buffer, endian);
    }

    BinaryFileReader(const char *file, Endian endian = BIG);
    BinaryFileReader(const std::string &file, Endian endian = BIG);
    operator bool() const;
};

class BinaryReader
{
public:
    const char *data;
    uint32_t index = 0;
    Endian endian;
    bool didAllocate;
    char bytes[8];
    BinaryBuffer buffer;

    template<typename T>
    T read()
    {
        return deserialize<T>(buffer, endian);
    }

    // copies contents, lifetime of buffer doesnt matter
    BinaryReader(const std::string &buffer, Endian endian = BIG);
    BinaryReader(const char *buffer, uint32_t bufferLen, Endian endian = BIG);
    // does not copy contents, 'buffer' must not be deleted until finished reading
    BinaryReader(const char *buffer, Endian endian = BIG);
    ~BinaryReader();

    BinaryReader(BinaryReader const &) = delete;
    BinaryReader& operator=(BinaryReader const &) = delete;
    BinaryReader(BinaryReader &&);
    BinaryReader& operator=(BinaryReader &&);
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
struct Vector2;
struct Vector3;
template<> uint8_t deserialize(BinaryBuffer&, Endian);
template<> int8_t deserialize(BinaryBuffer&, Endian);
template<> int16_t deserialize(BinaryBuffer&, Endian);
template<> uint16_t deserialize(BinaryBuffer&, Endian);
template<> int32_t deserialize(BinaryBuffer&, Endian);
template<> uint32_t deserialize(BinaryBuffer&, Endian);
template<> _Float32 deserialize(BinaryBuffer&, Endian);
template<> Vector2 deserialize(BinaryBuffer&, Endian);
template<> Vector3 deserialize(BinaryBuffer&, Endian);
template<> std::string deserialize(BinaryBuffer&, Endian);

template<> void BinaryWriter::write<uint8_t>(uint8_t data);
template<> void BinaryWriter::write<int8_t>(int8_t data);
template<> void BinaryWriter::write<int16_t>(int16_t data);
template<> void BinaryWriter::write<uint16_t>(uint16_t data);
template<> void BinaryWriter::write<int32_t>(int32_t data);
template<> void BinaryWriter::write<uint32_t>(uint32_t data);
template<> void BinaryWriter::write<_Float32>(_Float32 data);
template<> void BinaryWriter::write<Vector2>(Vector2 data);
template<> void BinaryWriter::write<Vector3>(Vector3 data);
template<> void BinaryWriter::write<std::string>(std::string data);