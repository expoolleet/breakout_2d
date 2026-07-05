#pragma once

#include <glm/glm.hpp>

namespace core {
inline constexpr float WORLD_WIDTH = 40.0f;
inline constexpr float WORLD_HEIGHT = 22.5f;  // 16:9 ratio

inline constexpr float EPSILON = 1e-6f;
inline constexpr float PI = 3.1415926535897932f;
inline constexpr float TAU = 2.0f * PI;

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

glm::vec2 lerp(glm::vec2 a, glm::vec2 b, float t);
glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t);
glm::vec4 lerp(glm::vec4 a, glm::vec4 b, float t);
float lerp(float a, float b, float t);

bool isEqualApprox(float a, float b);

bool isZeroApprox(float a);
};  // namespace core
