#include "view.h"
#include "settings.h"
#include <iostream>

BallView::BallView(Ball const& ball) : ball(ball), renderer(NULL) {}

void BallView::init(SDL_Renderer* const renderer) {
    this->renderer = renderer;
}

void BallView::draw() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &ball.rect);
}

void BallView::close() {
    renderer = NULL;
}

PaddleView::PaddleView(Paddle const& paddle) : paddle(paddle), renderer(NULL) {}

void PaddleView::init(SDL_Renderer* const renderer) {
    this->renderer = renderer;
}

void PaddleView::draw() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &paddle.rect);
}

void PaddleView::close() {
    renderer = NULL;
}

ScoreView::ScoreView(int const& score, Vec2 const& position) :
    score(score),
    view_score(-1),
    position(position),
    score_rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = 0,
        .h = 0
    },
    score_texture(NULL),
    renderer(NULL),
    font(NULL)
{}

void ScoreView::init(SDL_Renderer* const renderer, TTF_Font* const font) {
    this->renderer = renderer;
    this->font = font;
    update();
}

void ScoreView::update() {
    if (score_texture != NULL) {
        cleanup();
    }

    view_score = score;
    SDL_Surface* score_surface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), {0xFF, 0xFF, 0xFF, 0xFF});
    score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    SDL_FreeSurface(score_surface);

    int width, height;
    SDL_QueryTexture(score_texture, NULL, NULL, &width, &height);
    score_rect.w = width;
    score_rect.h = height;
}

void ScoreView::draw() {
    if (view_score != score) {
        update();
    }
    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
}

void ScoreView::cleanup() {
    SDL_DestroyTexture(score_texture);
    score_texture = NULL;
}

void ScoreView::close() {
    cleanup();
    font = NULL;
    renderer = NULL;
}

View::View(Model const& model) :
    window(NULL),
    renderer(NULL),
    font(NULL),
    ball_view{model.ball},
    paddle_one_view{model.paddle_one},
    paddle_two_view{model.paddle_two},
    score_one_view{model.score_one, Vec2{
        (Constants::SCREEN_WIDTH / 4.0f) - (Constants::FONT_SIZE / 4.0f),
        Constants::MARGIN
    }},
    score_two_view{model.score_two, Vec2{
        (Constants::SCREEN_WIDTH * 3.0f / 4.0f) - (Constants::FONT_SIZE / 4.0f),
        Constants::MARGIN
    }}
{}

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

    font = TTF_OpenFont("assets/fonts/bit5x3.ttf", Constants::FONT_SIZE);
    if (font == NULL) {
        std::cout << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    ball_view.init(renderer);
    paddle_one_view.init(renderer);
    paddle_two_view.init(renderer);
    score_one_view.init(renderer, font);
    score_two_view.init(renderer, font);

    return true;
}

void View::render() {
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(renderer);

    draw_net();
    ball_view.draw();
    paddle_one_view.draw();
    paddle_two_view.draw();
    score_one_view.draw();
    score_two_view.draw();

    SDL_RenderPresent(renderer);
}

void View::draw_net() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for (int y = 0; y < Constants::SCREEN_HEIGHT; y++) {
        if (y % (Constants::NET_SPACING * 2) < Constants::NET_SPACING) {
            SDL_RenderDrawPoint(renderer, Constants::SCREEN_WIDTH / 2, y);
        }
    }
}

void View::close() {
    ball_view.close();
    paddle_one_view.close();
    paddle_two_view.close();
    score_one_view.close();
    score_two_view.close();

    TTF_CloseFont(font);
    font = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
}