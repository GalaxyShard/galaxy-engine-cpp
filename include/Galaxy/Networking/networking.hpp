#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <deque>
#include <mutex>
#include <atomic>
#include <Galaxy/event.hpp>

class NetworkReader
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

    NetworkReader(std::string buffer) : buffer(buffer) {}
};
class NetworkWriter
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
typedef void(*ClientStatusCallback)(const Connection&);
class Server
{
private:
    static std::unique_ptr<Server> inst;
    std::vector<Connection> clients;
    std::unordered_map<std::string, void(*)(NetworkReader, Connection)> rpcs;
    int listener = -1, shutdownPipe = -1;

    std::thread serverThread;
    std::mutex clientMutex, queueMutex;
    std::deque<std::string> queuedMessages;
    std::unique_ptr<Listener> preRenderConn;

    ClientStatusCallback joinCallback;
    ClientStatusCallback leaveCallback;

    std::deque<std::string> internalMsgs;
    bool isActive = 0;

    Server() = default;
    static void server_thread();
    static void process_messages();

    friend class Client;
public:
    static const std::vector<Connection>& get_clients();
    static void set_join_callback(ClientStatusCallback func);
    static void set_leave_callback(ClientStatusCallback func);

    static bool start(unsigned short port);
    static void shutdown();

    static void send_all(const char *msg, const NetworkWriter &data);
    static void send(Connection conn, const char *msg, const NetworkWriter &data);
    static void register_rpc(std::string name, void(*func)(NetworkReader, Connection));

    static bool is_active();
};
class Client
{
public:
    enum ErrorCode : short { NONE, GENERAL };
private:
    static std::unique_ptr<Client> inst;
    std::unordered_map<std::string, void(*)(NetworkReader)> rpcs;
    int serverConn = -1, shutdownPipe = -1;

    std::unique_ptr<std::thread> clientThread;
    std::mutex queueMutex, connMutex, shutdownMutex;
    std::deque<std::string> queuedMessages;

    std::unique_ptr<Listener> preRenderConn;
    void(*shutdownCallback)();
    void(*errorCallback)();
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
    static bool start_as_host();
    static bool start(const char *ip, unsigned short port);
    static void shutdown();
    static void set_shutdown_callback(void(*func)());
    static void set_error_callback(void(*func)());

    static void register_rpc(std::string name, void(*func)(NetworkReader));
    static void send(const char *msg, const NetworkWriter &data);

    static bool is_active();
};