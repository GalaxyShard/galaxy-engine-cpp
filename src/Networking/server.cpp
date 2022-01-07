#include "internalnet.hpp"
#include <Galaxy/Renderer/renderer.hpp>

//enum MsgFlags { CLIENT_JOIN=1<<0, CLIENT_LEAVE=1<<1 };

std::unique_ptr<Server> Server::inst;
void Server::pre_render()
{
    {
        auto lock = std::lock_guard(inst->clientMutex);
        while (inst->internalMsgs.size())
        {
            NetworkReader reader = NetworkReader(inst->internalMsgs[0]);
            unsigned char msg = reader.read<unsigned char>();
            int fd = reader.read<int>();

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
        while(inst->queuedMessages.size())
        {
            std::string &msg = inst->queuedMessages[0];
            unsigned long index = msg.find('\0');
            std::string func = msg.substr(0, index); // dont add 1 to skip null terminator
            

            if (inst->rpcs.count(func))
                inst->rpcs[func](NetworkReader(msg.substr(index+1))); // add 1 to index to skip null terminator
            inst->queuedMessages.pop_front();
        }
    }
}
void Server::server_thread()
{
    std::vector<pollfd> descriptors;
    descriptors.push_back({.events=POLLIN, .fd=inst->listener});

    {
        int pipeFD[2];
        check_socket(pipe(pipeFD));
        
        int shutdownReader = pipeFD[0];
        inst->shutdownPipe = pipeFD[1];
        descriptors.push_back({.events=POLLIN, .fd=shutdownReader});
    }
    while(1)
    {
        int eventCount = poll(descriptors.data(), descriptors.size(), -1);
        if (eventCount == -1)
        {
            printf("error %d occured while polling%s\n", errno, strerror(errno));
            assert(false);
        }
        if (eventCount < 1) continue;
        
        if (descriptors[1].revents & POLLIN)
        {
            printf("Server Exit\n");
            close(descriptors[1].fd); // pipe reader
            return;
        }
        if (descriptors[0].revents & POLLIN)
        {
            sockaddr_storage clientAddr;
            socklen_t clientAddrSize = sizeof(clientAddr);
            int clientFD = accept(inst->listener, (sockaddr*)&clientAddr, &clientAddrSize);

            descriptors.push_back({
                .fd = clientFD,
                .events = POLLIN|POLLHUP
            });
            {
                auto lock = std::lock_guard(inst->clientMutex);
                inst->clients.push_back(Connection(clientFD));

                NetworkWriter writer;
                writer.write<unsigned char>(CLIENT_JOIN);
                writer.write<int>(clientFD);
                inst->internalMsgs.push_back(writer.get_buffer());
                //inst->msgFlags &= CLIENT_JOIN;
                //if (inst->joinCallback)
                //    inst->joinCallback(inst->clients.back());
            }
        }
        for (int i = 2; i < descriptors.size(); ++i)
        {
            pollfd &client = descriptors[i];
            if (client.revents & POLLHUP)
            {
                printf("Client disconnected\n");
                int clientFD = descriptors[i].fd;
                descriptors.erase(descriptors.begin()+i);
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
                //if (inst->leaveCallback)
                //    inst->leaveCallback(inst->clients[index]);
                inst->clients.erase(inst->clients.begin()+index);

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
                inst->queuedMessages.push_back(std::string(buffer, bytesRead));
            }
        }
    }
}
bool Server::start(unsigned short port)
{
    if (inst)
        return 0;
    
    inst = std::unique_ptr<Server>(new Server());
    addrinfo *list = get_addr_list(NULL, port);
    //addrinfo &info = *list; // todo: use a loop instead and break on success or nullptr


    bool errored=0;
    for (addrinfo *info = list; info!=0; info = info->ai_next)
    {
        if ((inst->listener = socket(info->ai_family, info->ai_socktype, info->ai_protocol))==-1)
        {
            if (!info->ai_next) errored = 1;
            continue;
        }
        int yes = 1;
        if (setsockopt(inst->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))==-1)
        {
            if (!info->ai_next) errored = 1;
            continue;
        }
        if (bind(inst->listener, info->ai_addr, info->ai_addrlen)==-1)
        {
            if (!info->ai_next) errored = 1;
            close(inst->listener);
            continue;
        }
    }
    //if ((inst->listener = socket(info.ai_family, info.ai_socktype, info.ai_protocol))==-1)
    //    return 0;
    //
    //{
    //    int yes = 1;
    //    if (setsockopt(inst->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))==-1)
    //        return 0;
    //}
    //if (bind(inst->listener, info.ai_addr, info.ai_addrlen)==-1)
    //    return 0;
//
    //if ((listen(inst->listener, 5))==-1)
    //    return 0;
    //freeaddrinfo(&info); // memory leak if there is an error
    
    freeaddrinfo(list);
    if (errored)
    {
        inst = 0;
        return 0;
    }

    if ((listen(inst->listener, 5))==-1)
    {
        close(inst->listener);
        inst = 0;
        return 0;
    }

    inst->serverThread = std::thread(server_thread);
    inst->preRenderConn = Renderer::pre_render().connect(&pre_render);
    return 1;
}
void Server::shutdown()
{
    if (!inst)
        return;
    
    int val=1;
    write(inst->shutdownPipe, &val, 4);
    close(inst->shutdownPipe);
    close(inst->listener);
    {
        auto lock = std::lock_guard(inst->clientMutex);
        for (Connection conn : inst->clients) close(conn.fd);
    }
    inst->serverThread.join();
    inst = 0;
}
void Server::send_all(const char *msg, const NetworkWriter &data)
{
    for(Connection conn : inst->clients)
    {
        send(conn, msg, data);
    }
}
void Server::send(Connection conn, const char *msg, const NetworkWriter &data)
{
    if (!is_active())
    {
        fprintf(stderr, "Error: send must be called after starting server\n");
        return;
    }
    if (conn.fd == HOST_FD)
    {
        // directly run the function
        Client::inst->rpcs[msg](NetworkReader(data.get_buffer()));
    }
    else
    {
        std::string buffer = std::string(msg)+'\0'+data.get_buffer();
        long bytesSent = ::send(conn.fd, buffer.c_str(), buffer.size(), 0);
        check_socket(bytesSent);
    }
}
void Server::set_join_callback(ClientStatusCallback func)
{
    inst->joinCallback = func;
}
void Server::set_leave_callback(ClientStatusCallback func)
{
    inst->leaveCallback = func;
}
void Server::register_rpc(std::string name, void(*func)(NetworkReader))
{
    if (!is_active())
    {
        fprintf(stderr, "Error: rpcs must be registered after starting server\n");
        return;
    }
    inst->rpcs[name] = func;
}
bool Server::is_active()
{
    return inst.get();
}