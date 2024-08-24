#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/OS/defines.hpp>
#include <Galaxy/print.hpp>

#include "internalnet.hpp"

std::unique_ptr<Client> Client::inst;
void Client::pre_render()
{
#if !OS_WEB
    {
        auto lock = std::lock_guard(inst->queueMutex);
        while(inst->queuedMessages.size())
        {
            std::string &msg = inst->queuedMessages[0];
            unsigned long index = msg.find('\0');
            if (index == std::string::npos)
            {
                logerr("Message \"%o\" from server was corrupted\n", msg);
                inst->queuedMessages.pop_front();
                continue;
            }
            std::string func = msg.substr(0, index); // dont add 1 to exclude null terminator
            
            if (inst->rpcs.count(func))
                inst->rpcs[func](NetworkReader(msg.substr(index+1))); // add 1 to index to skip null terminator
            inst->queuedMessages.pop_front();
        }
    }
    auto lock = std::lock_guard(inst->shutdownMutex);
    if (inst->shuttingDown)
    {
        shutdown();
    }
#endif
}
void Client::client_thread(const char *ip, unsigned short port)
{
#if !OS_WEB
    {
        addrinfo *list = get_addr_list(ip, port);
        
        bool errored=0;
        for (addrinfo *info = list; info!=0; info = info->ai_next)
        {
            if ((inst->serverConn = socket(info->ai_family, info->ai_socktype, info->ai_protocol))==-1)
            {
                if (!info->ai_next) errored = 1;
                continue;
            }
            if (connect(inst->serverConn, info->ai_addr, info->ai_addrlen)==-1)
            {
                if (!info->ai_next) errored = 1;
                close(inst->serverConn);
                continue;
            }
        }
        freeaddrinfo(list);
        if (errored)
        {
            auto lock = std::lock_guard(inst->shutdownMutex);
            inst->shuttingDown = 1;
            inst->errorCode = GENERAL;
            return;
        }
    }
    std::vector<pollfd> descriptors;
    const int readerIndex = 0, connIndex = 1;
    {
        int pipeFD[2];
        if (pipe(pipeFD)==-1)
        {
            auto lock = std::lock_guard(inst->shutdownMutex);
            inst->shuttingDown = 1;
            inst->errorCode = GENERAL;
            return;
        }
        int shutdownReader = pipeFD[0];
        inst->shutdownPipe = pipeFD[1];
        descriptors.push_back({.events=POLLIN, .fd=shutdownReader});
    }
    descriptors.push_back({.events=POLLIN|POLLHUP, .fd=inst->serverConn});
    inst->isActive = 1;
    while(1)
    {
        int eventCount = poll(descriptors.data(), descriptors.size(), -1);
        if (eventCount == -1)
        {
            logerr("error %o occured while polling%o\n", errno, strerror(errno));
            auto lock = std::lock_guard(inst->shutdownMutex);
            inst->shuttingDown = 1;
            inst->errorCode = GENERAL;
            return;
        }
        if (eventCount < 1) continue;
        
        if (descriptors[readerIndex].revents & POLLIN)
        {
            logmsg("Client shutdown\n");
            close(descriptors[readerIndex].fd); // pipe reader
            return;
        }
        if (descriptors[connIndex].revents & POLLHUP)
        {
            if (inst->shuttingDown)
                return;
            logmsg("Server closed connection\n");
            auto lock = std::lock_guard(inst->shutdownMutex);
            inst->shuttingDown = 1;
        }
        else if (descriptors[connIndex].revents & POLLIN)
        {
            constexpr int bufferSize = 512;
            char buffer[bufferSize];
            int bytesRead=0;

            {
                auto lock = std::lock_guard(inst->connMutex);
                bytesRead = recv(inst->serverConn, buffer, bufferSize, 0);
            }
            check_socket(bytesRead);

            auto lock = std::lock_guard(inst->queueMutex);
            inst->queuedMessages.push_back(std::string(buffer, bytesRead));
        }
    }
#endif
}
void Client::start(const char *ip, unsigned short port, Callback errorCallback)
{
#if !OS_WEB
    if (inst)
        return;

    inst = std::unique_ptr<Client>(new Client()); // private ctor, no make_unique
    inst->errorCallback = errorCallback;
    
    inst->clientThread = std::make_unique<std::thread>(client_thread, ip, port);
    inst->preRenderConn = Renderer::pre_render().connect(&pre_render);
#endif
}
void Client::start_as_host(Callback errorCallback)
{
#if !OS_WEB
    if (inst || !Server::inst)
        return;
    
    inst = std::unique_ptr<Client>(new Client()); // private ctor, no make_unique
    inst->serverConn = HOST_FD;

    // Fake a message to the server
    NetworkWriter writer;
    writer.write<unsigned char>(CLIENT_JOIN);
    writer.write<int>(HOST_FD);
    Server::inst->internalMsgs.push_back(writer.get_buffer());
    inst->isActive = 1;
    Server::inst->clients.push_back(HOST_FD);
#endif
}
void Client::set_shutdown_callback(void(*func)())
{
#if !OS_WEB
    if (!inst)
        return logerr("set_shutdown_callback called before starting client\n");
    inst->shutdownCallback = func;
#endif
}
void Client::shutdown()
{
#if !OS_WEB
    if (!inst)
        return;

    inst->shutdownCallback();
    inst->isActive = 0;
    {
        char val=1;
        write(inst->shutdownPipe, &val, 1);
        close(inst->shutdownPipe);
    }
    {
        auto lock = std::lock_guard(inst->connMutex);
        if (inst->serverConn >= 0)
        {
            close(inst->serverConn);
            inst->serverConn = -1;
        }
    }
    if (inst->clientThread && inst->clientThread->joinable())
        inst->clientThread->join();
    
    if (Server::inst)
    {
        NetworkWriter writer;
        writer.write<unsigned char>(CLIENT_LEAVE);
        writer.write<int>(HOST_FD);
        Server::inst->internalMsgs.push_back(writer.get_buffer());
        auto &clients = Server::inst->clients;
        for (unsigned int i = 0; i < clients.size(); ++i)
        {
            if (clients[i].fd == HOST_FD)
            {
                clients.erase(clients.begin()+i);
                break;
            }
        }
    }
    inst = 0;
#endif
}
void Client::register_rpc(std::string name, ArgCallback<const NetworkReader&> func)
{
#if !OS_WEB
    if (!inst)
        return logerr("register_rpc called before starting client\n");
    inst->rpcs[name] = func;
#endif
}
void Client::send(const char *msg, const NetworkWriter &data)
{
#if !OS_WEB
    if (!inst)
        return logerr("send called before starting client\n");
    if (Server::inst)
    {
        // directly run function if host
        Server::inst->rpcs[msg](NetworkReader(data.get_buffer()), Connection(HOST_FD));
    }
    else
    {
        std::string buffer = std::string(msg)+'\0'+data.get_buffer();
        long bytesSent = ::send(inst->serverConn, buffer.c_str(), buffer.size(), 0);
        check_socket(bytesSent);
    }
#endif
}
bool Client::is_active()
{
    return inst && inst->isActive;
}