#include <Galaxy/Math/vector.hpp>
#include <Galaxy/init.hpp>

#include "internalnet.hpp"

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
static void clean()
{
    Client::shutdown();
    Server::shutdown();
}
CLEANUP_FUNC(clean);