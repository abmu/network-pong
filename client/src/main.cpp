#include "controller.h"
#include "network.h"
#include <iostream>

int main(int argc, char* args[]) {
    Network network;
    network.init("127.0.0.1", 10224);
    network.send_data("hello");
    std::cout << network.recv_data() << std::endl;
    network.close_sock();
    return 0;



    Controller controller;
    if (!controller.init()) {
        std::cout << "Failed to initialise" << std::endl;
        return 1;
    }
    controller.run();
    controller.close();
    return 0;
}