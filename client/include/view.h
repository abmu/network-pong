#pragma once
#include <SDL2/SDL.h>

class View {
    public:
        View();
        bool init();
        void render();
        void close();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
};