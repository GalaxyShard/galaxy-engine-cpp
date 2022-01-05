#include "internalnet.hpp"
#include <Galaxy/Math/vector.hpp>

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
        exit(1);
    }
    return linkedList;
}
int check_socket(int status)
{
    if (status == -1)
    {
        fprintf(stderr, "socket error %d: %s\n", errno, strerror(errno));
        assert(false);
    }
    return status;
}

template<>
char NetworkReader::read<char>()
{
    nextIndex += 1;
    return buffer[nextIndex-1];
}
template<>
float NetworkReader::read<float>()
{
    // todo: optimize for IEEE instead of ASCII
    std::string val;
    while(1)
    {
        if (buffer[nextIndex] == '\0')
            break;
        val.push_back(buffer[nextIndex]);
        ++nextIndex;
    }
    // for some reason this wouldnt work, even though it is the equivalent to the above
    //for (;buffer[nextIndex]=='\0';++nextIndex)
    //{
    //    val.push_back(buffer[nextIndex]);
    //}
    ++nextIndex; // skip past \0
    return std::stof(val);
    //nextIndex += 1;
    //return buffer[nextIndex-1];
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
void NetworkWriter::write<char>(char data)
{
    buffer.push_back(data);
}

template<>
void NetworkWriter::write<float>(float data)
{
    // todo: optimize for IEEE
    buffer.append(std::to_string(data)+'\0');
}
template<>
void NetworkWriter::write<Vector3>(Vector3 data)
{
    write(data.x);
    write(data.y);
    write(data.z);
}