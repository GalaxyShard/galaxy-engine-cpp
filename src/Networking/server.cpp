#include <Galaxy/Renderer/renderer.hpp>

#include "internalnet.hpp"

std::unique_ptr<Server> Server::inst;
void Server::process_messages()
{
#if !OS_WEB
    {
        while (inst->internalMsgs.size())
        {
            NetworkReader reader = NetworkReader(inst->internalMsgs[0]);
            unsigned char msg = reader.read<unsigned char>();
            int fd = reader.read<int>();

            logmsg("Client %o\n", msg == CLIENT_JOIN ? "joined" : "left");
            if (msg == CLIENT_JOIN)
            {
                if (inst->joinCallback)
                    inst->joinCallback(Connection(fd));
            }
            else if (msg == CLIENT_LEAVE)
            {
                if (inst->leaveCallback)
                    inst->leaveCallback(Connection(fd));
            }
            inst->internalMsgs.pop_front();
        }
    }
    {
        auto lock = std::lock_guard(inst->queueMutex);
        while (inst->queuedMessages.size())
        {
            std::string &msg = inst->queuedMessages[0];
            int connID = b_cast<int>(&msg[0]);
            //memcpy(&connID, &msg[0], 4);
            //int connID = *(int *)&msg[0];
            unsigned long index = msg.find('\0', sizeof(connID));
            if (index == std::string::npos)
            {
                /*
                    connID is guarenteed to be correct, even though
                    message was corrupt, because it is applied on the
                    server right before queueing message
                */
                logerr("Message \"%o\" from client %o was corrupted\n", msg.c_str(), connID);
                inst->queuedMessages.pop_front();
                continue;
            }
            std::string func = msg.substr(sizeof(connID), index - sizeof(connID));

            if (inst->rpcs.count(func))
                inst->rpcs[func](NetworkReader(msg.substr(index+1)), Connection(connID)); // add 1 to index to skip null terminator
            inst->queuedMessages.pop_front();
        }
    }
#endif
}
void Server::server_thread()
{
#if !OS_WEB
    std::vector<pollfd> descriptors;
    descriptors.push_back({.events = POLLIN, .fd = inst->listener});

    {
        int pipeFD[2];
        check_socket(pipe(pipeFD));

        int shutdownReader = pipeFD[0];
        inst->shutdownPipe = pipeFD[1];
        descriptors.push_back({.events = POLLIN, .fd = shutdownReader});
    }
    inst->isActive = 1;
    while (1)
    {
        int eventCount = poll(descriptors.data(), descriptors.size(), -1);
        if (eventCount == -1)
        {
            logmsg("error %o occured while polling%o\n", errno, strerror(errno));
            assert(false);
        }
        if (eventCount < 1)
            continue;

        if (descriptors[1].revents & POLLIN)
        {
            logmsg("Server Exit\n");
            close(descriptors[1].fd); // pipe reader
            return;
        }
        if (descriptors[0].revents & POLLIN)
        {
            sockaddr_storage clientAddr;
            socklen_t clientAddrSize = sizeof(clientAddr);
            int clientFD = accept(inst->listener, (sockaddr *)&clientAddr, &clientAddrSize);

            descriptors.push_back({.fd = clientFD,
                                   .events = POLLIN | POLLHUP});
            {
                auto lock = std::lock_guard(inst->clientMutex);
                inst->clients.push_back(Connection(clientFD));

                NetworkWriter writer;
                writer.write<unsigned char>(CLIENT_JOIN);
                writer.write<int>(clientFD);
                inst->internalMsgs.push_back(writer.get_buffer());
            }
        }
        for (int i = 2; i < descriptors.size(); ++i)
        {
            pollfd &client = descriptors[i];
            if (client.revents & POLLHUP)
            {
                logmsg("Client disconnected\n");
                auto lock = std::lock_guard(inst->clientMutex);
                int clientFD = descriptors[i].fd;
                descriptors.erase(descriptors.begin() + i);
                --i;
                int index = -1;
                for (int i = 0; i < inst->clients.size(); ++i)
                {
                    if (inst->clients[i].fd == clientFD)
                    {
                        index = i;
                        break;
                    }
                }
                close(clientFD);
                inst->clients.erase(inst->clients.begin() + index);

                NetworkWriter writer;
                writer.write<unsigned char>(CLIENT_LEAVE);
                writer.write<int>(clientFD);
                inst->internalMsgs.push_back(writer.get_buffer());
            }
            else if (client.revents & POLLIN)
            {
                constexpr int bufferSize = 512;
                char buffer[bufferSize];
                int bytesRead = recv(client.fd, buffer, bufferSize, 0);
                check_socket(bytesRead);

                auto lock = std::lock_guard(inst->queueMutex);
                inst->queuedMessages.push_back(std::string((const char *)&client.fd, 4) + std::string(buffer, bytesRead));
            }
        }
    }
#endif
}
bool Server::start(unsigned short port)
{
#if !OS_WEB
    if (inst)
        return 0;

    inst = std::unique_ptr<Server>(new Server());
    addrinfo *list = get_addr_list(NULL, port);

    bool errored = 0;
    for (addrinfo *info = list; info != 0; info = info->ai_next)
    {
        if ((inst->listener = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1)
        {
            if (!info->ai_next)
                errored = 1;
            continue;
        }
        int yes = 1;
        if (setsockopt(inst->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        {
            if (!info->ai_next)
                errored = 1;
            continue;
        }
        if (bind(inst->listener, info->ai_addr, info->ai_addrlen) == -1)
        {
            if (!info->ai_next)
                errored = 1;
            close(inst->listener);
            continue;
        }
    }
    freeaddrinfo(list);
    if (errored)
    {
        inst = 0;
        return 0;
    }

    if ((listen(inst->listener, 5)) == -1)
    {
        close(inst->listener);
        inst = 0;
        return 0;
    }

    inst->serverThread = std::thread(server_thread);
    inst->preRenderConn = Renderer::pre_render().connect(&process_messages);
    return 1;
#else
    return 0;
#endif
}
void Server::shutdown()
{
#if !OS_WEB
    if (!inst)
        return;
    inst->isActive = 0;

    int val = 1;
    write(inst->shutdownPipe, &val, 4);
    close(inst->shutdownPipe);
    close(inst->listener);
    {
        auto lock = std::lock_guard(inst->clientMutex);
        for (Connection conn : inst->clients)
            if (conn.fd != HOST_FD)
                close(conn.fd);
        
        inst->clients.clear();
    }
    inst->serverThread.join();
    process_messages();
    inst = 0;
#endif
}
void Server::send_all(const char *msg, const NetworkWriter &data)
{
#if !OS_WEB
    auto lock = std::lock_guard(inst->clientMutex);
    for (Connection conn : inst->clients)
    {
        send(conn, msg, data);
    }
#endif
}
void Server::send(Connection conn, const char *msg, const NetworkWriter &data)
{
#if !OS_WEB
    if (!inst.get())
    {
        logerr("Error: send called before starting server\n");
        return;
    }
    if (conn.fd == HOST_FD)
    {
        // directly run the function
        Client::inst->rpcs[msg](NetworkReader(data.get_buffer()));
    }
    else
    {
        std::string buffer = std::string(msg) + '\0' + data.get_buffer();
        long bytesSent = ::send(conn.fd, buffer.c_str(), buffer.size(), 0);
        check_socket(bytesSent);
    }
#endif
}
const std::vector<Connection> &Server::get_clients()
{
#if !OS_WEB
    if (!inst.get())
    {
        logerr("Error: Server must be active to get clients\n");
    }
    auto lock = std::lock_guard(inst->clientMutex);
    return inst->clients;
#else
    throw("Not supported on web");
#endif
}
void Server::set_join_callback(ClientStatusCallback func)
{
#if !OS_WEB
    inst->joinCallback = func;
#endif
}
void Server::set_leave_callback(ClientStatusCallback func)
{
#if !OS_WEB
    inst->leaveCallback = func;
#endif
}
//void Server::register_rpc(std::string name, void (*func)(NetworkReader, Connection))
void Server::register_rpc(std::string name, ArgCallback<NetworkReader, Connection> func)
{
#if !OS_WEB
    if (!inst.get())
    {
        logerr("Error: rpc registered before starting server\n");
        return;
    }
    inst->rpcs[name] = func;
#endif
}
bool Server::is_active()
{
    return inst.get() && inst->isActive;
}