#include "model.h"
#include <iostream>
#include <cmath>

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

Ball::Ball() :
    rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = Constants::BALL_SIZE,
        .h = Constants::BALL_SIZE
    }
{}

void Ball::update(float ball_x, float ball_y, float ball_vel_x, float ball_vel_y) {
    position.x = ball_x * Constants::SCALE;
    position.y = ball_y * Constants::SCALE;
    rect.x = static_cast<int>(std::round(position.x));
    rect.y = static_cast<int>(std::round(position.y));
    velocity.x = ball_vel_x * Constants::SCALE;
    velocity.y = ball_vel_y * Constants::SCALE;
}

void Ball::move(float dt) {
    position += velocity * dt;
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

Paddle::Paddle() :
    rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = Constants::PADDLE_WIDTH,
        .h = Constants::PADDLE_HEIGHT
    }
{}

void Paddle::update(float paddle_x, float paddle_y, float paddle_vel_x, float paddle_vel_y) {
    position.x = paddle_x * Constants::SCALE;
    position.y = paddle_y * Constants::SCALE;
    rect.x = static_cast<int>(std::round(position.x));
    rect.y = static_cast<int>(std::round(position.y));
    velocity.x = paddle_vel_x * Constants::SCALE;
    velocity.y = paddle_vel_y * Constants::SCALE;
}

void Paddle::move(float dt) {
    position += velocity * dt;
    if (position.y < 0) {
        position.y = 0;
    } else if (position.y > Constants::SCREEN_HEIGHT - rect.h) {
        position.y = Constants::SCREEN_HEIGHT - rect.h;
    }
    rect.x = static_cast<int>(std::round(position.x));
    rect.y = static_cast<int>(std::round(position.y));
}

Model::Model() :
    ball{},
    paddle_one{},
    paddle_two{},
    score_one(0),
    score_two(0)
{}

void Model::update_scores(int score_one, int score_two) {
    this->score_one = score_one;
    this->score_two = score_two;
}

void Model::update(float dt) {
    paddle_one.move(dt);
    paddle_two.move(dt);
    ball.move(dt);
}