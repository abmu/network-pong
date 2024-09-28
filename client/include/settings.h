#pragma once

namespace Constants {
    inline constexpr float SCALE = 1.0f;
    inline constexpr int MARGIN = static_cast<int>(45 * SCALE);
    inline constexpr int SCREEN_WIDTH = static_cast<int>(960 * SCALE);
    inline constexpr int SCREEN_HEIGHT = static_cast<int>(720 * SCALE);
    inline constexpr int BALL_SIZE = static_cast<int>(10 * SCALE);
    inline constexpr float BALL_SPEED = 0.6f * SCALE;
    inline constexpr int PADDLE_WIDTH = static_cast<int>(10 * SCALE);
    inline constexpr int PADDLE_HEIGHT = static_cast<int>(90 * SCALE);
    inline constexpr float PADDLE_SPEED = 1.0f * SCALE;
    inline constexpr int FONT_SIZE = static_cast<int>(60 * SCALE);
    inline constexpr int NET_SPACING = static_cast<int>(20 * SCALE);
}

enum class Direction {
    NONE = 1,
    UP = 2,
    DOWN = 3,
    LEFT = 4,
    RIGHT = 5
};