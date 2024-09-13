#include "controller.h"
#include <iostream>

int main(int argc, char* args[]) {
    Controller controller;
    if (!controller.init()) {
        std::cout << "Failed to initialise" << std::endl;
        return 1;
    }
    controller.run();
    controller.close();
    return 0;
}