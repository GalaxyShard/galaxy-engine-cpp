#pragma once
#include <Galaxy/Networking/networking.hpp>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <thread>

constexpr static int HOST_FD = -2;
addrinfo* get_addr_list(const char *ip, unsigned short port);
int check_socket(int status);
