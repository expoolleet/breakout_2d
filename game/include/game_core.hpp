#pragma once

#include <glm/glm.hpp>

namespace core {
inline constexpr float WORLD_WIDTH = 40.0f;
inline constexpr float WORLD_HEIGHT = 22.5f;  // 16:19 ratio

constexpr float getWorldWidth() {
    return WORLD_WIDTH;
}

constexpr float getWorldHeight() {
    return WORLD_HEIGHT;
}

constexpr float getWorldAspectRatio() {
    return getWorldWidth() / getWorldHeight();
}

glm::vec4 getWorldAABB();

float getPixelsPerUnit();

glm::vec2 getWorldPosition(glm::vec2 screenPosition);

glm::vec2 getWorldPosition(float xNorm, float yNorm);

glm::vec2 getScreenPosition(float xNorm, float yNorm);

glm::vec2 getScreenPosition(glm::vec2 worldPosition);

glm::mat4 getProjectionMatrix();

glm::mat4 getScaledProjectionMatrix();
};  // namespace core
