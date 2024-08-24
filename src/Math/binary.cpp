#include <fstream>
#include <cstdint>

#include <Galaxy/Math/binary.hpp>
#include <Galaxy/Math/vector.hpp>


Endian sys_endian()
{
    constexpr int32_t one=1;
    static Endian endian = (*(uint8_t*)&one == 1) ? LITTLE : BIG;
    return endian;
}

bool read_bytes_regular(void *reader, char* buffer, uint32_t num)
{
    BinaryReader *reader0 = (BinaryReader*)reader;
    memcpy(buffer, reader0->data, num);
    reader0->index += num;
    return 1;
}
bool read_bytes_file(void *reader, char* buffer, uint32_t num)
{
    BinaryFileReader *reader0 = (BinaryFileReader*)reader;
    return (bool)reader0->stream.read(buffer, num);
}

BinaryReader::BinaryReader(const char *buffer, Endian endian)
    : data(buffer), endian(endian), didAllocate(0),
    buffer((void*)this, &read_bytes_regular, (char*)bytes) { }
BinaryReader::BinaryReader(const char *buffer, uint32_t bufferLen, Endian endian)
    : endian(endian), didAllocate(1),
    buffer((void*)this, &read_bytes_regular, (char*)bytes)
{
    char *bufferCpy = new char[bufferLen];
    memcpy(bufferCpy, buffer, bufferLen);
    data = bufferCpy;
}
BinaryReader::BinaryReader(const std::string &buffer, Endian endian)
    : endian(endian), didAllocate(1),
    buffer((void*)this, &read_bytes_regular, (char*)bytes)
{
    char *bufferCpy = new char[buffer.size()];
    buffer.copy(bufferCpy, buffer.size());
    data = bufferCpy;
}
BinaryReader::BinaryReader(BinaryReader &&other)
    : buffer((void*)this, &read_bytes_regular, (char*)bytes)
{
    data = other.data;
    index = other.index;
    endian = other.endian;
    didAllocate = other.didAllocate;

    other.didAllocate = 0;
}
BinaryReader& BinaryReader::operator=(BinaryReader &&other)
{
    if (this != &other){
        data = other.data;
        index = other.index;
        endian = other.endian;
        didAllocate = other.didAllocate;

        other.didAllocate = 0;
    }
    return *this;
}
BinaryReader::~BinaryReader()
{
    if (didAllocate)
    {
        delete[] (char*)data;
    }
}
BinaryFileReader::BinaryFileReader(const char *file, Endian endian)
    : stream(file, std::ios::binary), endian(endian),
    buffer((void*)this, &read_bytes_file, (char*)bytes) { }
BinaryFileReader::BinaryFileReader(const std::string &file, Endian endian)
    : stream(file, std::ios::binary), endian(endian),
    buffer((void*)this, &read_bytes_file, (char*)bytes) { }
BinaryFileReader::operator bool() const { return (bool)stream; }

template<typename T>
inline T deserialize_general(BinaryBuffer &buffer, Endian endian)
{
    static_assert(sizeof(T) <= 8, "temporary buffer is only 8 bytes");
    if (buffer.read(buffer.temp, sizeof(T)))
        return to_native_endian<T>(buffer.temp, endian);
    else return T();
}
template<>
int8_t deserialize<int8_t>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<int8_t>(buffer, endian);
}
template<>
uint8_t deserialize<uint8_t>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<uint8_t>(buffer, endian);
}
template<>
int16_t deserialize<int16_t>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<int16_t>(buffer, endian);
}
template<>
uint16_t deserialize<uint16_t>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<uint16_t>(buffer, endian);
}
template<>
int32_t deserialize<int32_t>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<int32_t>(buffer, endian);
}
template<>
uint32_t deserialize<uint32_t>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<uint32_t>(buffer, endian);
}
template<>
_Float32 deserialize<_Float32>(BinaryBuffer &buffer, Endian endian)
{
    static_assert(std::numeric_limits<_Float32>::is_iec559);
    return deserialize_general<_Float32>(buffer, endian);
}
template<>
Vector2 deserialize<Vector2>(BinaryBuffer &buffer, Endian endian)
{
    _Float32 x = deserialize<_Float32>(buffer, endian);
    _Float32 y = deserialize<_Float32>(buffer, endian);
    return Vector2(x,y);
}
template<>
Vector3 deserialize<Vector3>(BinaryBuffer &buffer, Endian endian)
{
    _Float32 x = deserialize<_Float32>(buffer, endian);
    _Float32 y = deserialize<_Float32>(buffer, endian);
    _Float32 z = deserialize<_Float32>(buffer, endian);
    return Vector3(x,y,z);
}
template<>
std::string deserialize<std::string>(BinaryBuffer &buffer, Endian endian)
{
    if (!buffer.read(buffer.temp, 4))
        return std::string();
    uint32_t length = to_native_endian<uint32_t>(buffer.temp, endian);
    std::string str;
    str.resize(length);
    if (!buffer.read(&str[0], length))
        return std::string();
    return str;
}

template<>
void BinaryWriter::write<int8_t>(int8_t data)
{
    buffer.push_back(data);
}
template<>
void BinaryWriter::write<uint8_t>(uint8_t data)
{
    buffer.push_back(data);
}
template<>
void BinaryWriter::write<int16_t>(int16_t data)
{
    int16_t newData = set_endian16<int16_t>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(int16_t));
}
template<>
void BinaryWriter::write<uint16_t>(uint16_t data)
{
    uint16_t newData = set_endian16<uint16_t>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(uint16_t));
}
template<>
void BinaryWriter::write<int32_t>(int32_t data)
{
    int32_t newData = set_endian32<int32_t>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(int32_t));
}
template<>
void BinaryWriter::write<uint32_t>(uint32_t data)
{
    uint32_t newData = set_endian32<uint32_t>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(uint32_t));
}

template<>
void BinaryWriter::write<_Float32>(_Float32 data)
{
    // IEEE-754
    static_assert(std::numeric_limits<_Float32>::is_iec559);
    uint8_t *rawBytes;
    rawBytes = (uint8_t*)&data;

    if (sys_endian() == LITTLE)
    {
        std::swap(rawBytes[0], rawBytes[3]);
        std::swap(rawBytes[1], rawBytes[2]);
    }
    buffer.append(std::string((char*)rawBytes, 4));
}
template<>
void BinaryWriter::write<Vector2>(Vector2 data)
{
    write(data.x);
    write(data.y);
}
template<>
void BinaryWriter::write<Vector3>(Vector3 data)
{
    write(data.x);
    write(data.y);
    write(data.z);
}

template<>
void BinaryWriter::write<std::string>(std::string data)
{
    write<uint32_t>(data.size());
    buffer += data;
}
