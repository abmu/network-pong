#include <iostream>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 720;

bool init();
void close();

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    screenSurface = SDL_GetWindowSurface(window);
    return true;
}

void close() {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if (!init()) {
        std::cout << "Failed to initialise" << std::endl;
        return 1;
    }

    // SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_UpdateWindowSurface(window);
    }

    close();
    
    return 0;
}