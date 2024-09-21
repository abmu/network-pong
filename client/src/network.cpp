#include "network.h"
#include <iostream>
#include <unistd.h>

Network::Network() : sock(-1), serv_addr{} {}

bool Network::init(std::string const& serv_ip, int serv_port) {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    return connect_serv();
}

bool Network::connect_serv() {
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection to server failed" << std::endl;
        return false;
    }
    std::cout << "Connection to server successful" << std::endl;
    return true;
}

bool Network::send_data(std::string const& data) {
    if (send(sock, data.c_str(), data.length(), 0) < 0) {
        std::cout << "Send failed" << std::endl;
        return false;
    }
    std::cout << "Send successful" << std::endl;
    return true;
}

std::string Network::recv_data() {
    char buffer[1024] = {};
    if (recv(sock, buffer, 1024, 0) < 0) {
        std::cout << "Receive failed" << std::endl;
        return "";
    }
    return std::string(buffer);
}

void Network::close_sock() {
    close(sock);
}