#pragma once
#include "network.h"
#include "model.h"
#include "view.h"
#include <SDL2/SDL.h>

class Controller {
    public:
        Controller();
        bool init();
        void run();
        void close();
    
    private:
        Network network;
        Model model;
        View view;
        bool running;
        void handle_events();
        void handle_keydown(SDL_Event event);
        void handle_keyup(SDL_Event event);
};