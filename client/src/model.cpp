#include "model.h"
#include "settings.h"

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

Ball::Ball(Vec2 position) :
    position(position),
    rect{
        .x = static_cast<int>(position.x),
        .y = static_cast<int>(position.y),
        .w = Constants::BALL_WIDTH,
        .h = Constants::BALL_HEIGHT
    }
{}

Model::Model() :
    ball{Vec2{
        (Constants::SCREEN_WIDTH / 2.0f) - (Constants::BALL_WIDTH / 2.0f),
        (Constants::SCREEN_HEIGHT / 2.0f) - (Constants::BALL_HEIGHT / 2.0f)
    }}
{}