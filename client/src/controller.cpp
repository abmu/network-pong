#include "controller.h"
#include <iostream>
#include <chrono>

Controller::Controller() : running(true), paddle_dir(Direction::NONE), view(model), network(model, paddle_dir) {}

bool Controller::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialise! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return view.init() && network.init("127.0.0.1", 9999);
}

void Controller::run() {
    float dt = 0.0f;
    while (running) {
        auto start_time = std::chrono::steady_clock::now();

        bool timeout = network.read();
        if (timeout) {
            std::cout << "Connection timed out. Game ended or server down" << std::endl;
            running = false;
            break;
        }
        handle_events();
        model.update(dt);
        network.write();
        view.render();

        auto stop_time = std::chrono::steady_clock::now();
        dt = std::chrono::duration<float, std::chrono::seconds::period>(stop_time - start_time).count();
        // float fps = 1000.0f / dt;
        // std::cout << "fps: " << fps << std::endl;
    }
}

void Controller::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            handle_keydown(event);
        } else if (event.type == SDL_KEYUP) {
            handle_keyup(event);
        }
    }
}

void Controller::handle_keydown(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
    } else if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
        paddle_dir = Direction::UP;
    } else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
        paddle_dir = Direction::DOWN;
    }
}

void Controller::handle_keyup(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
        if (paddle_dir == Direction::UP) {
            paddle_dir = Direction::NONE;
        }
    } else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
        if (paddle_dir == Direction::DOWN) {
            paddle_dir = Direction::NONE;
        }
    }
}

void Controller::close() {
    view.close();
    network.close_sock();
    TTF_Quit();
    SDL_Quit();
}