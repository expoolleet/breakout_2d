#pragma once

#include "powerup.hpp"
#include "powerup_type.hpp"

// PowerUp factory
class PowerUpFactory {
  public:
    static PowerUp createPowerUp(PowerUpType type, glm::vec2 position);
};
