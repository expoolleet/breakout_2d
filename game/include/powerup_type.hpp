#pragma once

#include "engine_context.hpp"
#include "texture_2d.hpp"

enum class PowerUpType {
    WidePlayer,
    StickyPlayer,
    PassTrough,
    FastBalls,
    None,
};

struct PowerUpData {
    glm::vec4 color;
    float duration;
    const Texture2D &texture;

    PowerUpData() : color(glm::vec4(1.0f)), duration(0.0f), texture(Context::get().textureManager->getTexture("")) {}
    PowerUpData(glm::vec4 col, float dur, const Texture2D &tex) : color(col), duration(dur), texture(tex) {}
};

inline const char *toString(PowerUpType type) {
    switch (type) {
        case PowerUpType::FastBalls:
            return "FastBalls";
        case PowerUpType::StickyPlayer:
            return "";
        case PowerUpType::PassTrough:
            return "PassTrough";
        case PowerUpType::WidePlayer:
            return "WidePlayer";
        case PowerUpType::None:
            return "None";
        default:
            return "Unknown";
    }
}

namespace {
inline PowerUpData getPowerUpData(PowerUpType type) {
    switch (type) {
        case PowerUpType::FastBalls:
            return PowerUpData(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), 10.0f, Context::get().textureManager->getTexture(STANDARD_BRICK));
        case PowerUpType::StickyPlayer:
            return PowerUpData(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), 10.0f, Context::get().textureManager->getTexture(STANDARD_BRICK));
        case PowerUpType::PassTrough:
            return PowerUpData(glm::vec4(0.25f, 0.75f, 0.0f, 1.0f), 3.0f, Context::get().textureManager->getTexture(STANDARD_BRICK));
        case PowerUpType::WidePlayer:
            return PowerUpData(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 10.0f, Context::get().textureManager->getTexture(STANDARD_BRICK));
        case PowerUpType::None:
        default:
            return PowerUpData();
    }
}
}  // namespace
