#pragma once
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

enum class Message : uint8_t {
    INIT = 1,
    INITACK = 2,
    HEARTBEAT = 3,
};

class Network{
    public:
        Network();
        int tick_rate;
        int heartbeat_sec;
        bool init(std::string const& server_ip, int server_port);
        bool send_msg(Message mesg);
        bool recv_data();
        void close_sock();

    private:
        int sock;
        sockaddr_in serv_addr;
        bool serv_handshake;
        bool handshake();
};