#pragma once

#include <glm/glm.hpp>

#include "ball.hpp"
#include "collision_type.hpp"
#include "powerup.hpp"
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

struct BallSpawned {
    BallPtr ball;
};

struct PlayerMoved {
    glm::vec2 position;
};

struct PowerUpSpawned {
    PowerUpType type;
    glm::vec2 position;
};

struct PowerUpActivated {
    PowerUpPtr powerUp;
};

struct PowerUpFinished {
    PowerUpPtr powerUp;
};

struct GameFinished {
    bool playerWon;
};
