#include <Galaxy/Math/binary.hpp>
#include <Galaxy/Math/vector.hpp>
#include <fstream>

// In case some platform doesnt use these values
static_assert(sizeof(int) == 4);
static_assert(sizeof(unsigned int) == 4);
static_assert(sizeof(short) == 2);
static_assert(sizeof(unsigned short) == 2);
static_assert(sizeof(long long) == 8);
static_assert(sizeof(unsigned long long) == 8);
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

static_assert((int)~0 == -1); // Two's complement
Endian sys_endian()
{
    constexpr intg one=1;
    static Endian endian = (*(ucharG*)&one == 1) ? LITTLE : BIG;
    return endian;
}

bool read_bytes_regular(void *reader, char* buffer, uintg num)
{
    BinaryReader *reader0 = (BinaryReader*)reader;
    memcpy(buffer, reader0->data, num);
    reader0->index += num;
    return 1;
}
bool read_bytes_file(void *reader, char* buffer, uintg num)
{
    BinaryFileReader *reader0 = (BinaryFileReader*)reader;
    return (bool)reader0->stream.read(buffer, num);
}

BinaryReader::BinaryReader(const char *buffer, Endian endian)
    : data(buffer), endian(endian), didAllocate(0),
    buffer((void*)this, &read_bytes_regular, (char*)bytes) { }
BinaryReader::BinaryReader(const char *buffer, uintg bufferLen, Endian endian)
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
scharG deserialize<scharG>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<scharG>(buffer, endian);
}
template<>
ucharG deserialize<ucharG>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<ucharG>(buffer, endian);
}
template<>
shortg deserialize<shortg>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<shortg>(buffer, endian);
}
template<>
ushortg deserialize<ushortg>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<ushortg>(buffer, endian);
}
template<>
intg deserialize<intg>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<intg>(buffer, endian);
}
template<>
uintg deserialize<uintg>(BinaryBuffer &buffer, Endian endian)
{
    return deserialize_general<uintg>(buffer, endian);
}
template<>
float deserialize<float>(BinaryBuffer &buffer, Endian endian)
{
    static_assert(std::numeric_limits<float>::is_iec559);
    return deserialize_general<float>(buffer, endian);
}
template<>
Vector2 deserialize<Vector2>(BinaryBuffer &buffer, Endian endian)
{
    float x = deserialize<float>(buffer, endian);
    float y = deserialize<float>(buffer, endian);
    return Vector2(x,y);
}
template<>
Vector3 deserialize<Vector3>(BinaryBuffer &buffer, Endian endian)
{
    float x = deserialize<float>(buffer, endian);
    float y = deserialize<float>(buffer, endian);
    float z = deserialize<float>(buffer, endian);
    return Vector3(x,y,z);
}
template<>
std::string deserialize<std::string>(BinaryBuffer &buffer, Endian endian)
{
    if (!buffer.read(buffer.temp, 4))
        return std::string();
    uintg length = to_native_endian<uintg>(buffer.temp, endian);
    std::string str;
    str.resize(length);
    if (!buffer.read(&str[0], length))
        return std::string();
    return str;
}

template<>
void BinaryWriter::write<scharG>(scharG data)
{
    buffer.push_back(data);
}
template<>
void BinaryWriter::write<ucharG>(ucharG data)
{
    buffer.push_back(data);
}
template<>
void BinaryWriter::write<shortg>(shortg data)
{
    shortg newData = set_endian16<shortg>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(shortg));
}
template<>
void BinaryWriter::write<ushortg>(ushortg data)
{
    ushortg newData = set_endian16<ushortg>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(ushortg));
}
template<>
void BinaryWriter::write<intg>(intg data)
{
    intg newData = set_endian32<intg>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(intg));
}
template<>
void BinaryWriter::write<uintg>(uintg data)
{
    uintg newData = set_endian32<uintg>((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(uintg));
}

template<>
void BinaryWriter::write<float>(float data)
{
    // IEEE-754
    static_assert(std::numeric_limits<float>::is_iec559);
    ucharG *rawBytes;
    rawBytes = (ucharG*)&data;

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
    write<uintg>(data.size());
    buffer += data;
}
