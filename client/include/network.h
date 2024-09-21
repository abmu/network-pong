#pragma once
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

class Network{
    public:
        Network();
        bool init(std::string const& server_ip, int server_port);
        bool send_data(std::string const& data);
        std::string recv_data();
        void close_sock();

    private:
        int sock;
        sockaddr_in serv_addr;
        bool connect_serv();
};