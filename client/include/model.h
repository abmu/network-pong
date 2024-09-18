#pragma once
#include "settings.h"
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

class Paddle{
    public:
        Paddle(Vec2 const& position);
        SDL_Rect rect;
        Vec2 position;
        void move(Direction direction);
        void stop(Direction direction);
        void update(float dt);

    private:
        Vec2 velocity;
        Direction direction;
};

class Ball{
    public:
        Ball(Vec2 const& position, Vec2 const& velocity);
        SDL_Rect rect;
        Direction direction;
        void check_paddle(Paddle const& paddle);
        void update(float dt);

    private:
        Vec2 position;
        Vec2 velocity;
};

class Model{
    public:
        Model();
        Ball ball;
        Paddle paddle_one;
        Paddle paddle_two;
        int score_one;
        int score_two;
        void update(float dt);

    private:
        void check_ball();
};