#pragma once

struct GameModifiers {
    bool stickyPlayer = false;
    bool widePlayer = false;
    bool narrowPlayer = false;
    bool godPlayer = false;

    bool passThrough = false;
    bool invisibleBalls = false;

    void reset() {
        *this = GameModifiers();
    }
};
