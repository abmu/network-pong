#pragma once
#include "view.h"
#include "model.h"
#include <SDL2/SDL.h>

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
        float dt;
        void handle_events();
        void handle_keydown(SDL_Event event);
};