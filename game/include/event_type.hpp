#pragma once

#include "powerup_type.hpp"

#include <glm/glm.hpp>

class Ball; // fwd

struct BallFliedOff {
    Ball &ball;
};

struct PlayerMoved {
    glm::vec2 position;
};

struct PowerUpActivated {
    PowerUpType type;
};

struct PowerUpFinished {
    PowerUpType type;
};
