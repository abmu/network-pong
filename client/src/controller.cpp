#include "controller.h"
#include <iostream>
#include <chrono>

Controller::Controller() : running(true), dt(0.0f), view(model) {}

bool Controller::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialise! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return view.init();
}

void Controller::run() {
    while (running) {
        auto start_time = std::chrono::high_resolution_clock::now();

        handle_events();
        model.update(dt);
        view.render();

        auto stop_time = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stop_time - start_time).count();
    }
}

void Controller::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            handle_keydown(event);
        }
    }
}

void Controller::handle_keydown(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
    } else if (event.key.keysym.sym == SDLK_w) {
        model.paddle_one.move(Constants::Direction::UP);
    } else if (event.key.keysym.sym == SDLK_s) {
        model.paddle_one.move(Constants::Direction::DOWN);
    }
}

void Controller::close() {
    view.close();
    TTF_Quit();
    SDL_Quit();
}