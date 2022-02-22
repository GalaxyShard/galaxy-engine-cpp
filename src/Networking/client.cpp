#include <Galaxy/Renderer/renderer.hpp>

#include "internalnet.hpp"

std::unique_ptr<Client> Client::inst;
void Client::pre_render()
{
    {
        auto lock = std::lock_guard(inst->queueMutex);
        while(inst->queuedMessages.size())
        {
            std::string &msg = inst->queuedMessages[0];
            unsigned long index = msg.find('\0');
            if (index == std::string::npos)
            {
                fprintf(stderr, "Message \"%s\" from server was corrupted\n", msg.c_str());
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
}
void Client::client_thread(const char *ip, unsigned short port)
{
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
            fprintf(stderr, "error %d occured while polling%s\n", errno, strerror(errno));
            
            auto lock = std::lock_guard(inst->shutdownMutex);
            inst->shuttingDown = 1;
            inst->errorCode = GENERAL;
            return;
        }
        if (eventCount < 1) continue;
        
        if (descriptors[readerIndex].revents & POLLIN)
        {
            printf("Client shutdown\n");
            close(descriptors[readerIndex].fd); // pipe reader
            return;
        }
        if (descriptors[connIndex].revents & POLLHUP)
        {
            if (inst->shuttingDown)
                return;
            printf("Server closed connection\n");
            auto lock = std::lock_guard(inst->shutdownMutex);
            inst->shuttingDown = 1;
        }
        else if (descriptors[connIndex].revents & POLLIN)
        {
            constexpr int bufferSize = 512;
            char buffer[bufferSize];
            int bytesRead;

            {
                auto lock = std::lock_guard(inst->connMutex);
                if (inst->serverConn >= 0)
                    bytesRead = recv(inst->serverConn, buffer, bufferSize, 0);
            }
            check_socket(bytesRead);

            auto lock = std::lock_guard(inst->queueMutex);
            inst->queuedMessages.push_back(std::string(buffer, bytesRead));
        }
    }
}
bool Client::start(const char *ip, unsigned short port)
{
    if (inst.get())
        return 0;
    
    inst = std::unique_ptr<Client>(new Client());
    
    inst->clientThread = std::make_unique<std::thread>(client_thread, ip, port);
    inst->preRenderConn = Renderer::pre_render().connect(&pre_render);
    return 1;
}
bool Client::start_as_host()
{
    if (inst.get() || !Server::inst.get())
        return 0;
    
    inst = std::unique_ptr<Client>(new Client());
    inst->serverConn = HOST_FD;

    // Fake a message to the server
    NetworkWriter writer;
    writer.write<unsigned char>(CLIENT_JOIN);
    writer.write<int>(HOST_FD);
    Server::inst->internalMsgs.push_back(writer.get_buffer());
    inst->isActive = 1;
    Server::inst->clients.push_back(HOST_FD);
    return 1;
}
void Client::set_shutdown_callback(void(*func)())
{
    if (!inst.get())
    {
        fprintf(stderr, "Error: shutdown callback set before starting client\n");
        return;
    }
    inst->shutdownCallback = func;
}
void Client::set_error_callback(void(*func)())
{
    if (!inst.get())
    {
        fprintf(stderr, "Error: error callback set before starting client\n");
        return;
    }
    inst->errorCallback = func;
}
void Client::shutdown()
{
    if (!inst.get())
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
    
    if (Server::inst.get())
    {
        NetworkWriter writer;
        writer.write<unsigned char>(CLIENT_LEAVE);
        writer.write<int>(HOST_FD);
        Server::inst->internalMsgs.push_back(writer.get_buffer());
        auto &clients = Server::inst->clients;
        for (int i = 0; i < clients.size(); ++i)
        {
            if (clients[i].fd == HOST_FD)
            {
                clients.erase(clients.begin()+i);
                break;
            }
        }
    }
    inst = 0;
}
void Client::register_rpc(std::string name, void(*func)(NetworkReader))
{
    if (!inst.get())
    {
        fprintf(stderr, "Error: rpc registered before starting client\n");
        return;
    }
    inst->rpcs[name] = func;
}
void Client::send(const char *msg, const NetworkWriter &data)
{
    if (!inst.get())
    {
        fprintf(stderr, "Error: send can only be called after starting client\n");
        return;
    }
    if (Server::inst.get())
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
}
bool Client::is_active()
{
    return inst.get() && inst->isActive;
}