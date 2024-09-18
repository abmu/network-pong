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
    velocity(velocity),
    direction(
        velocity.x > 0 ? Direction::RIGHT :
        velocity.x < 0 ? Direction::LEFT :
        Direction::NONE
    )
{}

void Ball::check_paddle(Paddle const& paddle) {
    float ball_bottom = position.y + Constants::BALL_HEIGHT;
    if (ball_bottom < paddle.position.y) {
        return;
    }

    if (position.y > paddle.position.y + Constants::PADDLE_HEIGHT) {
        return;
    }

    if (direction == Direction::RIGHT) {
        float ball_right = position.x + Constants::BALL_WIDTH;
        if (ball_right < paddle.position.x || ball_right > paddle.position.x + Constants::PADDLE_WIDTH) {
            return;
        } 
    } else if (direction == Direction::LEFT) {
        if (position.x > paddle.position.x + Constants::PADDLE_WIDTH || position.x < paddle.position.x) {
            return;
        }
    } else {
        return;
    }

    float paddle_one_third = paddle.position.y + (Constants::PADDLE_HEIGHT / 3.0f); 
    float paddle_two_third = paddle.position.y + (Constants::PADDLE_HEIGHT * 2.0f / 3.0f);
    if (ball_bottom < paddle_one_third) {
        handle_paddle_collide(CollisionType::PADDLE_TOP);
    } else if (position.y > paddle_two_third) {
        handle_paddle_collide(CollisionType::PADDLE_BOTTOM);
    } else {
        handle_paddle_collide(CollisionType::PADDLE_MIDDLE);
    }
}

void Ball::handle_paddle_collide(CollisionType collision) {
    velocity.x *= -1;
    direction = velocity.x > 0 ? Direction::RIGHT : Direction::LEFT;

    if (collision == CollisionType::PADDLE_TOP) {
        velocity.y = -0.75f * Constants::BALL_SPEED;
    } else if (collision == CollisionType::PADDLE_BOTTOM) {
        velocity.y = 0.75f * Constants::BALL_SPEED;
    }
}

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
    direction(Direction::NONE)
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

void Paddle::move(Direction direction) {
    this->direction = direction;
    if (this->direction == Direction::NONE){
        velocity.y = 0.0f;
    } else if (this->direction == Direction::UP) {
        velocity.y = -Constants::PADDLE_SPEED;
    } else if (this->direction == Direction::DOWN) {
        velocity.y = Constants::PADDLE_SPEED;
    }
}

void Paddle::stop(Direction direction) {
    if (this->direction == direction) {
        move(Direction::NONE);
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

void Model::check_ball() {
    if (ball.direction == Direction::RIGHT) {
        ball.check_paddle(paddle_two);
    } else if (ball.direction == Direction::LEFT) {
        ball.check_paddle(paddle_one);
    }
}

void Model::update(float dt) {
    paddle_one.update(dt);
    paddle_two.update(dt);
    check_ball();
    ball.update(dt);
}