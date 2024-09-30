#include "network.h"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

Network::Network(Model const& model, Direction const& paddle_dir) :
    model(model),
    paddle_dir(paddle_dir),
    sock(-1),
    serv_addr{},
    serv_handshake(false),
    timeout_ms(-1),
    send_ms(-1),
    last_recv(std::chrono::steady_clock::now()),
    last_send(std::chrono::steady_clock::now()),
    seq_num(0),
    last_game_seq_num(0)
{}

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

void Network::write() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    float time_since_send = std::chrono::duration<float, std::chrono::milliseconds::period>(now - last_send).count();
    if (time_since_send >= send_ms) {
        send_paddle_dir();
    }
}

bool Network::send_init() {
    int buff_size = 1;
    std::byte buffer[buff_size] = {};
    buffer[0] = static_cast<std::byte>(Message::INIT);
    return send_data(buffer, buff_size);
}

bool Network::send_heartbeat() {
    int buff_size = 1;
    std::byte buffer[buff_size] = {};
    buffer[0] = static_cast<std::byte>(Message::HEARTBEAT);
    return send_data(buffer, buff_size);
}

bool Network::send_paddle_dir() {
    seq_num++;
    int buff_size = 4;
    std::byte buffer[buff_size] = {};
    buffer[0] = static_cast<std::byte>(Message::PADDLE_DIR);
    buffer[1] = static_cast<std::byte>((seq_num >> 8) & 0xFF);
    buffer[2] = static_cast<std::byte>(seq_num & 0xFF);
    buffer[3] = static_cast<std::byte>(paddle_dir);
    return send_data(buffer, buff_size);
}

bool Network::send_data(std::byte* buffer, int buff_size) {
    if (send(sock, buffer, buff_size, 0) < 0) {
        std::cout << "Send failed" << std::endl;
        return false;
    }
    last_send = std::chrono::steady_clock::now();
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

bool Network::recv_data() {
    std::byte buffer[1024] = {};
    if (recv(sock, buffer, 1024, 0) < 0) {
        return false;
    }
    last_recv = std::chrono::steady_clock::now();
    parse_msg(buffer);
    return true;
}

void Network::parse_msg(std::byte* buffer) {
    Message msg = static_cast<Message>(buffer[0]);
    if (msg == Message::INITACK) {
        handle_initack(buffer);
    }
}

void Network::handle_initack(std::byte* buffer) {
    serv_handshake = true;
    timeout_ms = ntohs(*reinterpret_cast<uint16_t*>(&buffer[1]));
    int send_rate = static_cast<int>(buffer[3]);
    send_ms = 1000.0f / send_rate;
}

void Network::handle_model_update(std::byte* buffer) {
    uint16_t game_seq_num = ntohs(*reinterpret_cast<uint16_t*>(&buffer[1]));
    if (!ascending_seq_num(last_game_seq_num, game_seq_num)) {
        return;
    }
    last_game_seq_num = game_seq_num;
}

bool Network::ascending_seq_num(uint16_t seq_1, uint16_t seq_2) {
	uint16_t max = ~0;
	return (seq_1 - seq_2) % max > (seq_2 - seq_1) % max;
}


void Network::close_sock() {
    close(sock);
}