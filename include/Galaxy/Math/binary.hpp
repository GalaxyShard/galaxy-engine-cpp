#pragma once
#include <string>
#include <fstream>
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
    typedef bool(*read_bytes_func)(void *reader, char* buffer, uintg num);
    void *reader;
    read_bytes_func readBytes; // reads bytes to buffer
    char *temp; // 8 byte char array
    BinaryBuffer(void *reader, read_bytes_func readBytes, char *temp)
        : reader(reader), readBytes(readBytes), temp(temp) {}

    bool read(char* buffer, uintg num) { return readBytes(reader,buffer,num); }
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
    uintg index = 0;
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
    BinaryReader(const char *buffer, uintg bufferLen, Endian endian = BIG);
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
template<> ucharG deserialize(BinaryBuffer&, Endian);
template<> scharG deserialize(BinaryBuffer&, Endian);
template<> shortg deserialize(BinaryBuffer&, Endian);
template<> ushortg deserialize(BinaryBuffer&, Endian);
template<> intg deserialize(BinaryBuffer&, Endian);
template<> uintg deserialize(BinaryBuffer&, Endian);
template<> float deserialize(BinaryBuffer&, Endian);
template<> Vector2 deserialize(BinaryBuffer&, Endian);
template<> Vector3 deserialize(BinaryBuffer&, Endian);
template<> std::string deserialize(BinaryBuffer&, Endian);

template<> void BinaryWriter::write<ucharG>(ucharG data);
template<> void BinaryWriter::write<scharG>(scharG data);
template<> void BinaryWriter::write<shortg>(shortg data);
template<> void BinaryWriter::write<ushortg>(ushortg data);
template<> void BinaryWriter::write<intg>(intg data);
template<> void BinaryWriter::write<uintg>(uintg data);
template<> void BinaryWriter::write<float>(float data);
template<> void BinaryWriter::write<Vector2>(Vector2 data);
template<> void BinaryWriter::write<Vector3>(Vector3 data);
template<> void BinaryWriter::write<std::string>(std::string data);