#pragma once
#include "settings.h"
#include <SDL2/SDL.h>
#include <chrono>

class Vec2{
    public:        
        Vec2();
        Vec2(float x, float y);
        float x, y;
        Vec2 operator+(Vec2 const& rhs);
        Vec2& operator+=(Vec2 const& rhs);
        Vec2 operator*(float rhs);
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

enum class CollisionType {
    PADDLE_TOP,
    PADDLE_MIDDLE,
    PADDLE_BOTTOM
};

class Ball{
    public:
        Ball(Vec2 const& position, Vec2 const& velocity);
        SDL_Rect rect;
        Direction direction;
        void check_paddle(Paddle const& paddle);
        bool scored();
        void reset_position();
        void update(float dt);

    private:
        Vec2 const start_position;
        Vec2 position;
        Vec2 velocity;
        void reverse_direction();
        void handle_paddle_collide(CollisionType collision);
};

class Model{
    public:
        Model();
        Ball ball;
        Paddle paddle_one;
        Paddle paddle_two;
        int score_one;
        int score_two;
        void start_pause(int duration);
        void update(float dt);

    private:
        bool paused;
        std::chrono::steady_clock::time_point pause_end_time;
        void check_ball();
        void check_pause();
};