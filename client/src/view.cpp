#include "view.h"
#include "settings.h"
#include <iostream>

BallView::BallView(Ball& ball) : ball(ball), renderer(NULL) {}

void BallView::init(SDL_Renderer* renderer) {
    this->renderer = renderer;
}

void BallView::draw() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &ball.rect);
}

View::View(Model& model) : window(NULL), renderer(NULL), ball_view(model.ball) {}

bool View::init() {
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    ball_view.init(renderer);

    return true;
}

void View::render() {
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(renderer);

    draw_net();
    ball_view.draw();

    SDL_RenderPresent(renderer);
}

void View::draw_net() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for (int y = 0; y < Constants::SCREEN_HEIGHT; y++) {
        if (y % 5 != 0) {
            SDL_RenderDrawPoint(renderer, Constants::SCREEN_WIDTH / 2, y);
        }
    }
}

void View::close() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
}