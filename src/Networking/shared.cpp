#include "internalnet.hpp"
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/init.hpp>

addrinfo* get_addr_list(const char *ip, unsigned short port)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = PF_INET;

    addrinfo *linkedList;
    std::string portStr = std::to_string(port);
    int status = getaddrinfo(ip, portStr.c_str(), &hints, &linkedList);
    if (status != 0)
    {
        fprintf(stderr, "addrinfo error %d: %s\n", status, gai_strerror(status));
        assert(false);
    }
    return linkedList;
}
int check_socket(int status)
{
    if (status == -1)
    {
        fprintf(stderr, "socket error %d: %s\n", errno, strerror(errno));
    }
    return status;
}
// In case some platform doesnt use these values
static_assert(sizeof(int) == 4);
static_assert(sizeof(short) == 2);
static_assert(sizeof(long long) == 8);
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

template<>
unsigned char NetworkReader::read<unsigned char>()
{
    nextIndex += 1;
    return buffer[nextIndex-1];
}
template<>
signed char NetworkReader::read<signed char>()
{
    nextIndex += 1;
    return buffer[nextIndex-1];
}
template<>
int NetworkReader::read<int>()
{
    int val = *(int*)&buffer[nextIndex];
    nextIndex += 4;
    return ntohl(val);
}
template<>
std::string NetworkReader::read<std::string>()
{
    unsigned short length = *(unsigned short*)&buffer[nextIndex];
    length = ntohs(length);
    nextIndex += length+2;
    return buffer.substr(nextIndex-length, length);
}

template<>
float NetworkReader::read<float>()
{
    // IEEE-754
    static_assert(std::numeric_limits<float>::is_iec559);
    
    auto b0 = read<unsigned char>();
    auto b1 = read<unsigned char>();
    auto b2 = read<unsigned char>();
    auto b3 = read<unsigned char>();

    unsigned char rawBytes[4] = {b0,b1,b2,b3};
    int one = 1;
    if (*(unsigned char*)&one == 1)
    {
        std::swap(rawBytes[0], rawBytes[3]);
        std::swap(rawBytes[1], rawBytes[2]);
    }
    return *(float*)rawBytes;

    //// bytes are left to right
    //unsigned char b0 = read<unsigned char>();
    //unsigned char b1 = read<unsigned char>();
    //unsigned char b2 = read<unsigned char>();
    //unsigned char b3 = read<unsigned char>();
//
    //unsigned char sign = (b0 & 0b10000000) >> 7;
    //unsigned char exponent = ((b0 & 0b01111111) << 1) | ((b1 & 0b10000000) >> 7);
    //unsigned char fraction[3] = {b1,b2,b3};
//
    //float fractionSum=0;
    //for (int i = 1; i <= 23; ++i)
    //{
    //    if (fraction[i/8] & (0b10000000>>(i%8)))
    //        fractionSum += powf(2, -i);
    //}
    //if (sign)
    //    return -powf(2, (float)exponent-127) * (1 + fractionSum);
    //else
    //    return powf(2, (float)exponent-127) * (1 + fractionSum);
    //return (float)(sign ^ 0b00000001) * -powf(2, (float)exponent-127) * (1 + fractionSum);
    // val = (1-sign) * pow(2, E-127) * (1 + (pow(2, -i)))
}
template<>
Vector3 NetworkReader::read<Vector3>()
{
    float x = read<float>();
    float y = read<float>();
    float z = read<float>();
    return Vector3(x,y,z);
}


template<>
void NetworkWriter::write<signed char>(signed char data)
{
    buffer.push_back(data);
}
template<>
void NetworkWriter::write<unsigned char>(unsigned char data)
{
    buffer.push_back(data);
}
template<>
void NetworkWriter::write<int>(int data)
{
    int bigEndian = htonl(data);
    buffer.append((char*)&bigEndian, sizeof(int));
}
template<>
void NetworkWriter::write<std::string>(std::string data)
{
    unsigned short length = (unsigned short)data.size();
    length = htons(length);
    buffer += (*(char*)&length) + data;
}

template<>
void NetworkWriter::write<float>(float data)
{
    // IEEE-754
    static_assert(std::numeric_limits<float>::is_iec559);
    unsigned char *rawBytes;
    rawBytes = (unsigned char*)&data;

    int one = 1;
    if (*(unsigned char*)&one == 1)
    {
        std::swap(rawBytes[0], rawBytes[3]);
        std::swap(rawBytes[1], rawBytes[2]);
    }
    buffer.append(std::string((char*)rawBytes, 4));
}
template<>
void NetworkWriter::write<Vector3>(Vector3 data)
{
    write(data.x);
    write(data.y);
    write(data.z);
}

static void clean()
{
    Client::shutdown();
    Server::shutdown();
}
CLEANUP_FUNC(clean);