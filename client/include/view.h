#pragma once
#include "model.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class BallView {
    public:
        BallView(Ball const& ball);
        void init(SDL_Renderer* const renderer);
        void draw();
    
    private:
        Ball const& ball;
        SDL_Renderer* renderer;
};

class PaddleView {
    public:
        PaddleView(Paddle const& paddle);
        void init(SDL_Renderer* const renderer);
        void draw();
    
    private:
        Paddle const& paddle;
        SDL_Renderer* renderer;
};

class ScoreView {
    public:
        ScoreView(int const& score, Vec2 const& position);
        void init(SDL_Renderer* const renderer, TTF_Font* const font);
        void update();
        void draw();
        void cleanup();

    private:
        int const& score;
        Vec2 const position;
        SDL_Rect score_rect;
        SDL_Texture* score_texture;
        SDL_Renderer* renderer;
        TTF_Font* font;
};

class View {
    public:
        View(Model const& model);
        bool init();
        void render();
        void close();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        TTF_Font* font;
        BallView ball_view;
        PaddleView paddle_one_view;
        PaddleView paddle_two_view;
        ScoreView score_one_view;
        ScoreView score_two_view;
        void draw_net();
};