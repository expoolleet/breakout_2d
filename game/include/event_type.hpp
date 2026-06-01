#pragma once

#include <glm/glm.hpp>

#include "ball.hpp"
#include "collision_type.hpp"
#include "powerup_type.hpp"

struct BallFliedOff {
    BallPtr ball;
};

struct BallUnstuck {
    BallPtr ball;
};

struct BallStuck {
    BallPtr ball;
};

struct BallHit {
    BallPtr ball;
    glm::vec2 position;
    CollisionType collisionType;
};

struct PlayerMoved {
    glm::vec2 position;
};

struct PowerUpSpawned {
    PowerUpType type;
    glm::vec2 position;
};

struct PowerUpActivated {
    PowerUpType type;
};

struct PowerUpFinished {
    PowerUpType type;
};

struct GameFinished {
    bool playerWon;
};
