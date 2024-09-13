#pragma once
#include "view.h"

class Controller {
    public:
        Controller();
        bool init();
        void run();
        void close();
    
    private:
        View view;
        bool running;
        void handle_events();
};