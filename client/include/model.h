#pragma once
#include "settings.h"
#include <SDL2/SDL.h>
#include <chrono>

// 2D vector class for position and velocity
class Vec2{
    public:        
        Vec2();
        Vec2(float x, float y);
        Vec2 operator+(Vec2 const& rhs);
        Vec2& operator+=(Vec2 const& rhs);
        Vec2 operator*(float rhs);
        float x, y;
};

// Represents a paddle in the game
class Paddle{
    public:
        Paddle();
        SDL_Rect rect;
        void update(float paddle_x, float paddle_y, float paddle_vel_x, float paddle_vel_y);
        void move(float dt);

    private:
        Vec2 position;
        Vec2 velocity;
};

// Represents the ball in the game
class Ball{
    public:
        Ball();
        SDL_Rect rect;
        void update(float ball_x, float ball_y, float ball_vel_x, float ball_vel_y);
        void move(float dt);

    private:
        Vec2 position;
        Vec2 velocity;
};

// Main game model that tracks game state
class Model{
    public:
        Model();
        Paddle paddle_one;
        Paddle paddle_two;
        Ball ball;
        int score_one;
        int score_two;
        void update_scores(int score_one, int score_two);
        void update(float dt);
};