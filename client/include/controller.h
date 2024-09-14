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
        Model model;
        View view;
        bool running;
        void handle_events();
};