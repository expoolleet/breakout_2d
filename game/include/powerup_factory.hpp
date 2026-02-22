#pragma once

#include "powerup.hpp"
#include "powerup_type.hpp"

// PowerUp factory
namespace _pf {

inline PowerUp createPowerUp(PowerUpType type, glm::vec2 position) {
    PowerUpData data = getPowerUpData(type);
    return PowerUp(type, data.color, data.duration, data.texture, position, DEFAULT_POWERUP_SIZE);
}

} // namespace _pf
