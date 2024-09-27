#pragma once
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>

enum class Message {
    INIT = 1,
    INITACK = 2,
    HEARTBEAT = 3,
};

class Network{
    public:
        Network();
        bool init(std::string const& server_ip, int server_port);
        bool read();
        void handle_heartbeat();
        void close_sock();

    private:
        int sock;
        sockaddr_in serv_addr;
        bool serv_handshake;
        int heartbeat_ms;
        int timeout_ms;
        bool game_start;
        std::chrono::steady_clock::time_point last_recv;
        std::chrono::steady_clock::time_point last_hearbeat;
        bool set_sock_block(bool blocking);
        bool handshake();
        bool send_data(char* buffer, int buff_size);
        bool send_init();
        bool send_heartbeat();
        bool recv_data();
        void parse_msg(char* buffer);
};