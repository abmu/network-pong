#pragma once

namespace Constants {
    constexpr int SCREEN_WIDTH = 960;
    constexpr int SCREEN_HEIGHT = 720;
    constexpr int BALL_WIDTH = 15;
    constexpr int BALL_HEIGHT = 15;
    constexpr int PADDLE_WIDTH = 10;
    constexpr int PADDLE_HEIGHT = 100;
    constexpr int PADDLE_SPEED = 1;
    constexpr int FONT_SIZE = 60;

    enum class Direction {
        NONE,
        UP,
        DOWN
    };
}