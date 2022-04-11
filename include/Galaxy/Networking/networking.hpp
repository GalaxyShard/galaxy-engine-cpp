#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <deque>
#include <mutex>
#include <atomic>
#include <Galaxy/Events/event.hpp>
#include <Galaxy/Math/binary.hpp>

typedef BinaryReader NetworkReader;
typedef BinaryWriter NetworkWriter;

class Connection
{
private:
    int fd = -1;
    friend class Server;
    friend class Client;
    Connection() { }
    Connection(int fd) : fd(fd) { }
public:
    int getID() const { return fd; }
};
typedef ArgCallback<const Connection&> ClientStatusCallback;
class Server
{
private:
    static std::unique_ptr<Server> inst;
    std::vector<Connection> clients;
    std::unordered_map<std::string, ArgCallback<const NetworkReader&, Connection>> rpcs;
    int listener = -1, shutdownPipe = -1;

    std::thread serverThread;
    std::mutex clientMutex, queueMutex;
    std::deque<std::string> queuedMessages;
    Listener preRenderConn;

    ClientStatusCallback joinCallback;
    ClientStatusCallback leaveCallback;
    Callback errorCallback;

    std::deque<std::string> internalMsgs;
    bool isActive = 0;

    Server() = default;
    static void server_thread();
    static void process_messages();

    friend class Client;
public:
    static const std::vector<Connection>* get_clients();
    static void set_join_callback(ClientStatusCallback func);
    static void set_leave_callback(ClientStatusCallback func);

    static void start(unsigned short port, Callback errorCallback);
    static void shutdown();

    static void send_all(const char *msg, const NetworkWriter &data);
    static void send(Connection conn, const char *msg, const NetworkWriter &data);
    static void register_rpc(std::string name, ArgCallback<const NetworkReader&, Connection> func);

    static bool is_active();
};
class Client
{
public:
    enum ErrorCode : short { NONE, GENERAL };
private:
    static std::unique_ptr<Client> inst;
    std::unordered_map<std::string, ArgCallback<const NetworkReader&>> rpcs;
    int serverConn = -1, shutdownPipe = -1;

    std::unique_ptr<std::thread> clientThread;
    std::mutex queueMutex, connMutex, shutdownMutex;
    std::deque<std::string> queuedMessages;

    Listener preRenderConn;
    Callback errorCallback;
    void(*shutdownCallback)();
    bool shuttingDown=0;
    ErrorCode errorCode = NONE;
    bool isActive = 0;

    Client() = default;
    static void client_thread(const char *ip, unsigned short port);
    static void pre_render();

    friend class Server;
public:
    /*
        start_as_host fakes a connection to the server to speed up the game.
        Start the server before starting the host client
    */
    static void start_as_host(Callback errorCallback);
    static void start(const char *ip, unsigned short port, Callback errorCallback);
    static void shutdown();
    static void set_shutdown_callback(void(*func)());

    static void register_rpc(std::string name, ArgCallback<const NetworkReader&> func);
    static void send(const char *msg, const NetworkWriter &data);

    static bool is_active();
};