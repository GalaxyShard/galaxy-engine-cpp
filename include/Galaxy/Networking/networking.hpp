#pragma once


namespace Server
{
    void start(unsigned short port);
    void shutdown();

    void send_all(void *data, unsigned long bytes);
    void register_command(void(*func)(void *data));

    bool is_active();
    //void send(, void *data, size_t bytes);
    //extern bool isServer;
}
namespace Client
{
    void start(const char *ip, unsigned short port);
    void shutdown();

    void register_rpc(void(*func)(void *data));
    void send(void *data, unsigned long bytes);

    bool is_active();
    //extern bool isClient;
}