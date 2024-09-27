#include "network.h"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstdint>

Network::Network() : sock(-1), serv_addr{}, serv_handshake(false), heartbeat_ms(-1), timeout_ms(-1), game_start(false), last_recv(std::chrono::steady_clock::now()), last_hearbeat(std::chrono::steady_clock::now()) {}

bool Network::init(std::string const& serv_ip, int serv_port) {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }

    if (!set_sock_block(false)) {
        std::cout << "Failed to put socket into non-blocking mode" << std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    if (!handshake()) {
        close(sock);
        return false;
    }
    return true;
}

bool Network::set_sock_block(bool blocking) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        return false;
    }
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(sock, F_SETFL, flags) == 0);
}

bool Network::handshake() {
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection to server failed" << std::endl;
        return false;
    }

    int const max_attempts = 3;
    int attempt = 0; 
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        if (!send_init()) {
            continue;
        }

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int activity = select(sock + 1, &read_fds, nullptr, nullptr, &timeout);
        if (activity < 0) {
            std::cout << "Select failed" << std::endl;
        } else if (activity == 0) {
            std::cout << "Read timeout, no data received" << std::endl;
        } else {
            if (FD_ISSET(sock, &read_fds)) {
                while (recv_data());
                if (serv_handshake) {
                    std::cout << "Connection to server successful\nWaiting for game to start..." << std::endl;
                    return true;
                }
            }
        }
    }
    std::cout << "Bad handshake" << std::endl;
    return false;
}

bool Network::send_init() {
    int buff_size = 1;
    char buffer[buff_size] = {};
    buffer[0] = static_cast<char>(Message::INIT);
    return send_data(buffer, buff_size);
}

bool Network::send_heartbeat() {
    int buff_size = 1;
    char buffer[buff_size] = {};
    buffer[0] = static_cast<char>(Message::HEARTBEAT);
    return send_data(buffer, buff_size);
}

bool Network::send_data(char* buffer, int buff_size) {
    if (send(sock, buffer, buff_size, 0) < 0) {
        std::cout << "Send failed" << std::endl;
        return false;
    }
    return true;
}

void Network::parse_msg(char* buffer) {
    Message msg = static_cast<Message>(buffer[0]);
    if (msg == Message::INITACK) {
        serv_handshake = true;
        heartbeat_ms = ntohs(*reinterpret_cast<uint16_t*>(&buffer[1]));
        timeout_ms = ntohs(*reinterpret_cast<uint16_t*>(&buffer[3]));
    } else if (msg == Message::HEARTBEAT) {
    } else {
        if (!game_start) {
            game_start = true;
        }
    }
}

bool Network::recv_data() {
    char buffer[1024] = {};
    if (recv(sock, buffer, 1024, 0) < 0) {
        return false;
    }
    last_recv = std::chrono::steady_clock::now();
    parse_msg(buffer);
    return true;
}

bool Network::read() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    float time_since_recv = std::chrono::duration<float, std::chrono::milliseconds::period>(now - last_recv).count();
    if (time_since_recv >= timeout_ms) {
        return true;
    }

    while (recv_data());
    return false;
}

void Network::handle_heartbeat() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    float time_since_heartbeat = std::chrono::duration<float, std::chrono::milliseconds::period>(now - last_hearbeat).count();
    if (time_since_heartbeat >= heartbeat_ms) {
        if (send_heartbeat()) {
            last_hearbeat = now;
        }
    }
}

void Network::close_sock() {
    close(sock);
}