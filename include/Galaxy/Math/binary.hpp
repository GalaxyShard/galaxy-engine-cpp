#pragma once
#include <string>
enum Endian:unsigned char { LITTLE, BIG };

int set_endian32(char *buffer, Endian from, Endian to);
unsigned int set_endian_u32(char *buffer, Endian from, Endian to);

short set_endian16(char *buffer, Endian from, Endian to);
unsigned short set_endian_u16(char *buffer, Endian from, Endian to);

short to_native_endian16(char *buffer, Endian current);
unsigned short to_native_endian_u16(char *buffer, Endian current);

int to_native_endian32(char *buffer, Endian current);
unsigned int to_native_endian_u32(char *buffer, Endian current);

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
struct Vector3;
template<> unsigned char BinaryReader::read<unsigned char>();
template<> signed char BinaryReader::read<signed char>();
template<> int BinaryReader::read<int>();
template<> std::string BinaryReader::read<std::string>();
template<> float BinaryReader::read<float>();
template<> Vector3 BinaryReader::read<Vector3>();

template<> void BinaryWriter::write<signed char>(signed char data);
template<> void BinaryWriter::write<unsigned char>(unsigned char data);
template<> void BinaryWriter::write<int>(int data);
template<> void BinaryWriter::write<std::string>(std::string data);
template<> void BinaryWriter::write<float>(float data);
template<> void BinaryWriter::write<Vector3>(Vector3 data);