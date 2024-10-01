#pragma once

namespace Constants {
    inline constexpr float SCALE = 2.4f;
    inline constexpr int SCREEN_WIDTH = static_cast<int>(400 * SCALE);
    inline constexpr int SCREEN_HEIGHT = static_cast<int>(300 * SCALE);
    inline constexpr int MARGIN = static_cast<int>(30 * SCALE);
    inline constexpr int BALL_SIZE = static_cast<int>(10 * SCALE);
    inline constexpr int PADDLE_WIDTH = static_cast<int>(10 * SCALE);
    inline constexpr int PADDLE_HEIGHT = static_cast<int>(50 * SCALE);
    inline constexpr int FONT_SIZE = static_cast<int>(20 * SCALE);
    inline constexpr int NET_SPACING = static_cast<int>(10 * SCALE);
}

enum class Direction {
    NONE = 1,
    UP = 2,
    DOWN = 3,
    LEFT = 4,
    RIGHT = 5
};