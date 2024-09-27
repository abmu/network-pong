#include "controller.h"
#include <iostream>
#include <chrono>

Controller::Controller() : running(true), view(model) {}

bool Controller::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialise! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return network.init("127.0.0.1", 9999) && view.init();
}

void Controller::run() {
    float dt = 0.0f;
    // model.start_pause(1000);

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
        network.handle_heartbeat();
        view.render();

        auto stop_time = std::chrono::steady_clock::now();
        dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stop_time - start_time).count();
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
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            running = false;
            break;
        case SDLK_w:
            model.paddle_one.move(Direction::UP);
            break;
        case SDLK_s:
            model.paddle_one.move(Direction::DOWN);
            break;
        case SDLK_UP:
            model.paddle_two.move(Direction::UP);
            break;
        case SDLK_DOWN:
            model.paddle_two.move(Direction::DOWN);
            break;
        default:
            break;
    }
}

void Controller::handle_keyup(SDL_Event event) {
    switch (event.key.keysym.sym) {
        case SDLK_w:
            model.paddle_one.stop(Direction::UP);
            break;
        case SDLK_s:
            model.paddle_one.stop(Direction::DOWN);
            break;
        case SDLK_UP:
            model.paddle_two.stop(Direction::UP);
            break;
        case SDLK_DOWN:
            model.paddle_two.stop(Direction::DOWN);
            break;
        default:
            break;
    }
}

void Controller::close() {
    view.close();
    network.close_sock();
    TTF_Quit();
    SDL_Quit();
}