#include "network.h"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

Network::Network() : sock(-1), serv_addr{}, serv_handshake(false), tick_rate(-1), heartbeat_sec(-1) {}

bool Network::init(std::string const& serv_ip, int serv_port) {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    return handshake();
}

bool Network::handshake() {
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection to server failed" << std::endl;
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
    
    int const max_attempts = 3;
    int attempt = 0; 
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        if (!send_msg(Message::INIT)) {
            continue;
        }
        recv_data();
        if (serv_handshake) {
            timeout.tv_sec = 0;
            timeout.tv_usec = 1;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

            std::cout << "Connection to server successful" << std::endl;
            return true;
        }
    }
    std::cout << "Bad handshake" << std::endl;
    return false;
}

bool Network::send_msg(Message msg) {
    uint8_t msg_byte = static_cast<uint8_t>(msg);
    if (send(sock, &msg_byte, sizeof(msg_byte), 0) < 0) {
        std::cout << "Send failed" << std::endl;
        return false;
    }
    return true;
}

bool Network::recv_data() {
    char buffer[1024] = {};
    if (recv(sock, buffer, 1024, 0) < 0) {
        std::cout << "Receive failed" << std::endl;
        return false;
    }

    if (buffer[0] == static_cast<uint8_t>(Message::INITACK)) {
        serv_handshake = true;
        tick_rate = buffer[1];
        heartbeat_sec = buffer[2];
    }
    return true;
}

void Network::close_sock() {
    close(sock);
}