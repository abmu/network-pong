#include "controller.h"
#include <SDL2/SDL.h>
#include <iostream>

Controller::Controller() : running(true), view(model) {}

bool Controller::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return view.init();
}

void Controller::run() {
    while (running) {
        handle_events();
        view.render();
    }
}

void Controller::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE ) {
                running = false;
            }
        }
    }
}

void Controller::close() {
    view.close();
    SDL_Quit();
}