#pragma once
#include "network.h"
#include "model.h"
#include "view.h"
#include "settings.h"
#include <SDL2/SDL.h>
#include <string>

class Controller {
    public:
        Controller();
        bool init(std::string const& serv_ip, int serv_port);
        void run();
        void close();
    
    private:
        bool running;
        Direction paddle_dir;
        Model model;
        View view;
        Network network;
        void handle_events();
        void handle_keydown(SDL_Event event);
        void handle_keyup(SDL_Event event);
};