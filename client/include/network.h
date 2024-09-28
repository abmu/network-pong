#pragma once
#include "model.h"
#include "settings.h"
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <cstddef>

enum class Message {
    INIT = 1,
    INITACK = 2,
    HEARTBEAT = 3,
    PADDLE_DIR = 4
};

class Network{
    public:
        Network(Model const& model, Direction const& paddle_dir);
        bool init(std::string const& server_ip, int server_port);
        bool read();
        void write();
        void close_sock();

    private:
        Model const& model;
        Direction const& paddle_dir;
        int sock;
        sockaddr_in serv_addr;
        bool serv_handshake;
        int timeout_ms;
        int send_rate;
        float send_ms;
        std::chrono::steady_clock::time_point last_recv;
        std::chrono::steady_clock::time_point last_send;
        bool set_sock_block(bool blocking);
        bool handshake();
        bool send_data(std::byte* buffer, int buff_size);
        bool send_init();
        bool send_heartbeat();
        bool send_paddle_dir();
        bool recv_data();
        void parse_msg(std::byte* buffer);
};