#include "model.h"
#include <iostream>
#include <cmath>

Vec2::Vec2() : x(0.0f), y(0.0f) {}

Vec2::Vec2(float x, float y) : x(x), y(y) {}

Ball::Ball(Vec2 const& position, Vec2 const& velocity) :
    position(position),
    rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = Constants::BALL_SIZE,
        .h = Constants::BALL_SIZE
    },
    velocity(velocity)
{}

void Ball::update(float dt) {
    float const ymax = Constants::SCREEN_HEIGHT - rect.h;
    if (position.y > ymax) {
        position.y = ymax - (position.y - ymax);
        velocity.y *= -1;
    } else if (position.y < 0) {
        position.y = -position.y;
        velocity.y *= -1;
    }
    rect.x = static_cast<int>(std::round(position.x));
    rect.y = static_cast<int>(std::round(position.y));
}

Paddle::Paddle(Vec2 const& position) :
    position(position),
    rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = Constants::PADDLE_WIDTH,
        .h = Constants::PADDLE_HEIGHT
    },
    velocity{},
    direction(Direction::NONE)
{}

void Paddle::update(float dt) {
    if (position.y < 0) {
        position.y = 0;
    } else if (position.y > Constants::SCREEN_HEIGHT - rect.h) {
        position.y = Constants::SCREEN_HEIGHT - rect.h;
    }
    rect.x = static_cast<int>(std::round(position.x));
    rect.y = static_cast<int>(std::round(position.y));
}

Model::Model() :
    ball{
        Vec2{
            (Constants::SCREEN_WIDTH / 2.0f) - (Constants::BALL_SIZE / 2.0f),
            (Constants::SCREEN_HEIGHT / 2.0f) - (Constants::BALL_SIZE / 2.0f)
        },
        Vec2{Constants::BALL_SPEED, 0.0f}
    },
    paddle_one{Vec2{
        Constants::MARGIN - (Constants::PADDLE_WIDTH / 2.0f),
        (Constants::SCREEN_HEIGHT / 2.0f) - (Constants::PADDLE_HEIGHT / 2.0f)
    }},
    paddle_two{Vec2{
        (Constants::SCREEN_WIDTH - Constants::MARGIN) - (Constants::PADDLE_WIDTH / 2.0f),
        (Constants::SCREEN_HEIGHT / 2.0f) - (Constants::PADDLE_HEIGHT / 2.0f)
    }},
    score_one(0),
    score_two(0)
{}

void Model::update(float dt) {
    paddle_one.update(dt);
    paddle_two.update(dt);
    ball.update(dt);
}