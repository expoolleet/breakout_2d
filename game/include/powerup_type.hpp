#pragma once

#include "texture_manager.hpp"

class Texture2D; // fwd

struct PowerUpData {
    glm::vec3 color;
    float duration;
    const Texture2D &texture;

    PowerUpData() : color(glm::vec3(1.0f)), duration(0.0f), texture(TextureManager::getTexture("")) {}
    PowerUpData(glm::vec3 color, float duration, const Texture2D &texture) : color(color), duration(duration), texture(texture) {}
};

enum PowerUpType {
    PowerUp_None,
    PowerUp_WidePlayer,
    PowerUp_StickyPlayer,
    PowerUp_PassTrough,
    PowerUp_FastBalls
};

inline const char *toString(PowerUpType type) {
    switch (type) {
    case PowerUp_FastBalls:
        return "PowerUp_FastBalls";
    case PowerUp_StickyPlayer:
        return "PowerUp_StickyPlayer";
    case PowerUp_PassTrough:
        return "PowerUp_PassTrough";
    case PowerUp_WidePlayer:
        return "PowerUp_WidePlayer";
    case PowerUp_None:
        return "PowerUp_None";
    }
}

namespace {
inline PowerUpData getPowerUpData(PowerUpType type) {
    switch (type) {
    case PowerUp_FastBalls:
        return PowerUpData(glm::vec3(1.0f, 0.0f, 1.0f), 10.0f, TextureManager::getTexture(STANDARD_BRICK));
    case PowerUp_StickyPlayer:
        return PowerUpData(glm::vec3(0.5f, 0.0f, 0.0f), 5.0f, TextureManager::getTexture(STANDARD_BRICK));
    case PowerUp_PassTrough:
        return PowerUpData(glm::vec3(0.0f, 1.0f, 0.0f), 3.0f, TextureManager::getTexture(STANDARD_BRICK));
    case PowerUp_WidePlayer:
        return PowerUpData(glm::vec3(0.0f, 0.0f, 1.0f), 10.0f, TextureManager::getTexture(STANDARD_BRICK));
    case PowerUp_None:
        return PowerUpData();
    }
}
} // namespace
