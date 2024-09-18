#pragma once

namespace Constants {
    constexpr float SCALE = 1.0f;
    constexpr int MARGIN = static_cast<int>(45 * SCALE);
    constexpr int SCREEN_WIDTH = static_cast<int>(960 * SCALE);
    constexpr int SCREEN_HEIGHT = static_cast<int>(720 * SCALE);
    constexpr int BALL_SIZE = static_cast<int>(10 * SCALE);
    constexpr float BALL_SPEED = 0.6f * SCALE;
    constexpr int PADDLE_WIDTH = static_cast<int>(10 * SCALE);
    constexpr int PADDLE_HEIGHT = static_cast<int>(90 * SCALE);
    constexpr float PADDLE_SPEED = 1.0f * SCALE;
    constexpr int FONT_SIZE = static_cast<int>(60 * SCALE);
    constexpr int NET_SPACING = static_cast<int>(20 * SCALE);
}

enum class Direction {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};