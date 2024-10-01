#include "controller.h"
#include <iostream>
#include <cstdint>

int main(int argc, char* args[]) {
    if (argc != 3) {
        std::cout << "Usage: " << args[0] << " <IP Address> <Port>" << std::endl;
        return 1;
    }
    std::string serv_ip = args[1];
    int serv_port = static_cast<uint16_t>(std::stoi(args[2]));

    Controller controller;
    if (!controller.init(serv_ip, serv_port)) {
        std::cout << "Failed to initialise" << std::endl;
        return 1;
    }
    controller.run();
    controller.close();
    return 0;
}