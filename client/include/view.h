#pragma once
#include "model.h"
#include <SDL2/SDL.h>

class BallView {
    public:
        BallView(Ball& ball);
        void init(SDL_Renderer* renderer);
        void draw();
    
    private:
        Ball ball;
        SDL_Renderer* renderer;
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
        BallView ball_view;
        void draw_net();
};