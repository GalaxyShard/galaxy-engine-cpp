#include <Galaxy/Math/binary.hpp>
#include <Galaxy/Math/vector.hpp>

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
    int one=1;
    return (*(unsigned char*)&one == 1) ? LITTLE : BIG;
}
int set_endian32(char *buffer, Endian from, Endian to)
{
    if (from != to)
    {
        std::swap(buffer[0], buffer[3]);
        std::swap(buffer[1], buffer[2]);
    }
    return b_cast<int>(buffer);
}
unsigned int set_endian_u32(char *buffer, Endian from, Endian to)
{
    if (from != to)
    {
        std::swap(buffer[0], buffer[3]);
        std::swap(buffer[1], buffer[2]);
    }
    return b_cast<unsigned int>(buffer);
}
float set_endian_f(char *buffer, Endian from, Endian to)
{
    if (from != to)
    {
        std::swap(buffer[0], buffer[3]);
        std::swap(buffer[1], buffer[2]);
    }
    return b_cast<float>(buffer);
}
short set_endian16(char *buffer, Endian from, Endian to)
{
    if (from != to)
        std::swap(buffer[0], buffer[1]);

    return b_cast<short>(buffer);
}
unsigned short set_endian_u16(char *buffer, Endian from, Endian to)
{
    if (from != to)
        std::swap(buffer[0], buffer[1]);

    return b_cast<unsigned short>(buffer);
}
int to_native_endian32(char *buffer, Endian current)
{
    return set_endian32(buffer, current, sys_endian());
}
unsigned int to_native_endian_u32(char *buffer, Endian current)
{
    return set_endian_u32(buffer, current, sys_endian());
}
float to_native_endian_f(char *buffer, Endian current)
{
    return set_endian_f(buffer, current, sys_endian());
}
short to_native_endian16(char *buffer, Endian current)
{
    return set_endian16(buffer, current, sys_endian());
}
unsigned short to_native_endian_u16(char *buffer, Endian current)
{
    return set_endian16(buffer, current, sys_endian());
}


template<>
signed char BinaryReader::read<signed char>()
{
    nextIndex += sizeof(signed char);
    return buffer[nextIndex-sizeof(signed char)];
}
template<>
unsigned char BinaryReader::read<unsigned char>()
{
    nextIndex += sizeof(unsigned char);
    return buffer[nextIndex-sizeof(unsigned char)];
}

template<>
short BinaryReader::read<short>()
{
    nextIndex += sizeof(short);
    return to_native_endian16(&buffer[nextIndex-sizeof(short)], Endian::BIG);
}
template<>
unsigned short BinaryReader::read<unsigned short>()
{
    nextIndex += sizeof(unsigned short);
    return to_native_endian_u16(&buffer[nextIndex-sizeof(unsigned short)], Endian::BIG);
}

template<>
int BinaryReader::read<int>()
{
    nextIndex += sizeof(int);
    return to_native_endian32(&buffer[nextIndex-sizeof(int)], Endian::BIG);
}
template<>
unsigned int BinaryReader::read<unsigned int>()
{
    nextIndex += sizeof(unsigned int);
    return to_native_endian_u32(&buffer[nextIndex-sizeof(unsigned int)], Endian::BIG);
}

template<>
float BinaryReader::read<float>()
{
    // Force IEEE-754
    static_assert(std::numeric_limits<float>::is_iec559);
    
    unsigned char rawBytes[4];
    rawBytes[0] = read<unsigned char>();
    rawBytes[1] = read<unsigned char>();
    rawBytes[2] = read<unsigned char>();
    rawBytes[3] = read<unsigned char>();
    if (sys_endian() == LITTLE)
    {
        std::swap(rawBytes[0], rawBytes[3]);
        std::swap(rawBytes[1], rawBytes[2]);
    }
    return b_cast<float>(rawBytes);
}
template<>
Vector2 BinaryReader::read<Vector2>()
{
    float x = read<float>();
    float y = read<float>();
    return Vector2(x,y);
}
template<>
Vector3 BinaryReader::read<Vector3>()
{
    float x = read<float>();
    float y = read<float>();
    float z = read<float>();
    return Vector3(x,y,z);
}

template<>
std::string BinaryReader::read<std::string>()
{
    unsigned char length = read<unsigned char>();
    nextIndex += length;
    return buffer.substr(nextIndex-length, length);
}


template<>
void BinaryWriter::write<signed char>(signed char data)
{
    buffer.push_back(data);
}
template<>
void BinaryWriter::write<unsigned char>(unsigned char data)
{
    buffer.push_back(data);
}
template<>
void BinaryWriter::write<short>(short data)
{
    short newData = set_endian16((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(short));
}
template<>
void BinaryWriter::write<unsigned short>(unsigned short data)
{
    unsigned short newData = set_endian_u16((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(unsigned short));
}
template<>
void BinaryWriter::write<int>(int data)
{
    int newData = set_endian32((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(int));
}
template<>
void BinaryWriter::write<unsigned int>(unsigned int data)
{
    unsigned int newData = set_endian_u32((char*)&data, sys_endian(), Endian::BIG);
    buffer.append((char*)&newData, sizeof(unsigned int));
}

template<>
void BinaryWriter::write<float>(float data)
{
    // IEEE-754
    static_assert(std::numeric_limits<float>::is_iec559);
    unsigned char *rawBytes;
    rawBytes = (unsigned char*)&data;

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
    write((unsigned char)data.size());
    buffer += data;
}
