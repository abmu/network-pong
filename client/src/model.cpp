#include "model.h"
#include <iostream>

Vec2::Vec2() : x(0.0f), y(0.0f) {}

Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2 Vec2::operator+(Vec2 const& rhs) {
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2& Vec2::operator+=(Vec2 const& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vec2 Vec2::operator*(float rhs) {
    return Vec2(x * rhs, y * rhs);
}

Ball::Ball(Vec2 const& position, Vec2 const& velocity) :
    position(position),
    rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = Constants::BALL_WIDTH,
        .h = Constants::BALL_HEIGHT
    },
    velocity(velocity)
{}

void Ball::update(float dt) {
    position += velocity * dt;
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
    direction(Constants::Direction::NONE)
{}

void Paddle::update(float dt) {
    position += velocity * dt;
    if (position.y < 0) {
        position.y = 0;
    } else if (position.y > (Constants::SCREEN_HEIGHT - Constants::PADDLE_HEIGHT)) {
        position.y = Constants::SCREEN_HEIGHT - Constants::PADDLE_HEIGHT;
    }
    rect.x = static_cast<int>(std::round(position.x));
    rect.y = static_cast<int>(std::round(position.y));
}

void Paddle::move(Constants::Direction direction) {
    this->direction = direction;
    if (this->direction == Constants::Direction::UP) {
        velocity.y = -Constants::PADDLE_SPEED;
    } else if (this->direction == Constants::Direction::DOWN) {
        velocity.y = Constants::PADDLE_SPEED;
    } else {
        velocity.y = 0.0f;
    }
}

void Paddle::stop(Constants::Direction direction) {
    if (this->direction == direction) {
        move(Constants::Direction::NONE);
    }
}

Model::Model() :
    ball{
        Vec2{
            (Constants::SCREEN_WIDTH / 2.0f) - (Constants::BALL_WIDTH / 2.0f),
            (Constants::SCREEN_HEIGHT / 2.0f) - (Constants::BALL_HEIGHT / 2.0f)
        },
        Vec2{static_cast<float>(Constants::BALL_SPEED), 0.0f}
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
    ball.update(dt);
    paddle_one.update(dt);
    paddle_two.update(dt);
}