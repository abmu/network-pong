#pragma once

namespace Constants {
    constexpr float SCALE = 1.0f;
    constexpr int MARGIN = static_cast<int>(50 * SCALE);
    constexpr int SCREEN_WIDTH = static_cast<int>(960 * SCALE);
    constexpr int SCREEN_HEIGHT = static_cast<int>(720 * SCALE);
    constexpr int BALL_WIDTH = static_cast<int>(15 * SCALE);
    constexpr int BALL_HEIGHT = static_cast<int>(15 * SCALE);
    constexpr int PADDLE_WIDTH = static_cast<int>(10 * SCALE);
    constexpr int PADDLE_HEIGHT = static_cast<int>(100 * SCALE);
    constexpr float PADDLE_SPEED = 0.75f * SCALE;
    constexpr int FONT_SIZE = static_cast<int>(60 * SCALE);
    constexpr int NET_SPACING = static_cast<int>(20 * SCALE);

    enum class Direction {
        NONE,
        UP,
        DOWN
    };
}