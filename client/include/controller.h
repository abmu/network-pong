#pragma once
#include "view.h"
#include "model.h"

class Controller {
    public:
        Controller();
        bool init();
        void run();
        void close();
    
    private:
        View view;
        Ball ball;
        bool running;
        void handle_events();
};