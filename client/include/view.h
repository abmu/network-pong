#pragma once
#include "model.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class BallView {
    public:
        BallView(Ball& ball);
        void init(SDL_Renderer* renderer);
        void draw();
    
    private:
        Ball ball;
        SDL_Renderer* renderer;
};

class PaddleView {
    public:
        PaddleView(Paddle& paddle);
        void init(SDL_Renderer* renderer);
        void draw();
    
    private:
        Paddle paddle;
        SDL_Renderer* renderer;
};

class ScoreView {
    public:
        ScoreView(int* score);
        void init(TTF_Font* font);
        void draw();

    private:
        int* score;
        TTF_Font* font;
};

class View {
    public:
        View(Model& model);
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