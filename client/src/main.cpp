#include "controller.h"
#include <iostream>

int main(int argc, char* args[]) {
    // allow user to input ip addr and port via command line args
    // add movement interpolation

    Controller controller;
    if (!controller.init()) {
        std::cout << "Failed to initialise" << std::endl;
        return 1;
    }
    controller.run();
    controller.close();
    return 0;
}