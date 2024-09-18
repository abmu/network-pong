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
    start_position(position),
    position(position),
    rect{
        .x = static_cast<int>(std::round(position.x)),
        .y = static_cast<int>(std::round(position.y)),
        .w = Constants::BALL_SIZE,
        .h = Constants::BALL_SIZE
    },
    velocity(velocity),
    direction(
        velocity.x > 0 ? Direction::RIGHT :
        velocity.x < 0 ? Direction::LEFT :
        Direction::NONE
    )
{}

void Ball::update(float dt) {
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

void Ball::check_paddle(Paddle const& paddle) {
    float const ball_bottom = position.y + rect.h;
    if (ball_bottom < paddle.position.y) {
        return;
    }

    if (position.y > paddle.position.y + paddle.rect.h) {
        return;
    }

    if (direction == Direction::RIGHT) {
        float const ball_right = position.x + rect.w;
        if (ball_right < paddle.position.x || ball_right > paddle.position.x + paddle.rect.w) {
            return;
        }
    } else if (direction == Direction::LEFT) {
        float const paddle_right = paddle.position.x + paddle.rect.w;
        if (position.x > paddle_right || position.x < paddle.position.x) {
            return;
        }
    } else {
        return;
    }

    CollisionType collision;
    if (ball_bottom < paddle.position.y + (paddle.rect.h / 3.0f)) {
        collision = CollisionType::PADDLE_TOP;
    } else if (position.y > paddle.position.y + (paddle.rect.h * 2.0f / 3.0f)) {
        collision = CollisionType::PADDLE_BOTTOM;
    } else {
        collision = CollisionType::PADDLE_MIDDLE;
    }

    handle_paddle_collide(collision);
}

void Ball::handle_paddle_collide(CollisionType collision) {
    if (collision == CollisionType::PADDLE_TOP) {
        velocity.y = -Constants::BALL_SPEED;
    } else if (collision == CollisionType::PADDLE_BOTTOM) {
        velocity.y = Constants::BALL_SPEED;
    }
    reverse_direction();
}

void Ball::reverse_direction() {
    velocity.x *= -1;
    direction = velocity.x > 0 ? Direction::RIGHT : Direction::LEFT;
}

bool Ball::scored() {
    if (position.x < -rect.w || position.x > Constants::SCREEN_WIDTH) {
        return true;
    }
    return false;
}

void Ball::reset_position() {
    position.x = start_position.x;
    position.y = start_position.y;
    reverse_direction();
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
    } else if (position.y > Constants::SCREEN_HEIGHT - rect.h) {
        position.y = Constants::SCREEN_HEIGHT - rect.h;
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
            (Constants::SCREEN_WIDTH / 2.0f) - (Constants::BALL_SIZE / 2.0f),
            (Constants::SCREEN_HEIGHT / 2.0f) - (Constants::BALL_SIZE / 2.0f)
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
    if (ball.scored()) {
        if (ball.direction == Direction::RIGHT) {
            score_one++;
        } else if (ball.direction == Direction::LEFT) {
            score_two++;
        }
        ball.reset_position();
        return;
    }

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