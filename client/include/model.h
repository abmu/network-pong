#pragma once
#include <SDL2/SDL.h>

class Vec2{
    public:        
        Vec2();
        Vec2(float x, float y);
        Vec2 operator+(Vec2 const& rhs);
        Vec2& operator+=(Vec2 const& rhs);
        Vec2 operator*(float rhs);
        float x, y;
};

class Ball{
    public:
        Ball(Vec2 position);
        Vec2 position;
        SDL_Rect rect;
};

class Model{
    public:
        Model();
        Ball ball;
};