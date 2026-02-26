#pragma once

#include "collision_type.hpp"
#include "powerup_type.hpp"

#include <glm/glm.hpp>

class Ball; // fwd

struct BallFliedOff {
    Ball &ball;
};

struct BallUnstuck {
    Ball &ball;
};

struct BallStuck {
    Ball &ball;
};

struct BallHit {
    glm::vec2 position;
    Ball &ball;
    CollisionType collisionType;
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
