#include "game_core.hpp"

#include <glm/glm.hpp>

#include "window.hpp"

glm::vec4 core::getWorldAABB() {
    float halfWidth = getWorldWidth() * 0.5f;
    float halfHeight = getWorldHeight() * 0.5f;
    return glm::vec4(-halfWidth, -halfHeight, halfWidth, halfHeight);
}

float core::getPixelsPerUnit() {
    return static_cast<float>(Window::getWidth()) / getWorldWidth();
}

glm::vec2 core::getWorldPosition(glm::vec2 screenPosition) {
    float w = static_cast<float>(Window::getWidth());
    float h = static_cast<float>(Window::getHeight());
    return glm::vec2((screenPosition.x / w - 0.5f) * getWorldWidth(), (0.5f - screenPosition.y / h) * getWorldHeight());
}

glm::vec2 core::getWorldPosition(float xNorm, float yNorm) {
    float halfWidth = getWorldWidth() / 2.0f;
    float halfHeight = getWorldHeight() / 2.0f;
    return glm::vec2(lerp(-halfWidth, halfWidth, xNorm), lerp(-halfHeight, halfHeight, yNorm));
}

glm::vec2 core::getScreenPosition(float xNorm, float yNorm) {
    return glm::vec2(xNorm * static_cast<float>(Window::getWidth()), yNorm * static_cast<float>(Window::getHeight()));
}

glm::vec2 core::getScreenPosition(glm::vec2 worldPosition) {
    float w = static_cast<float>(Window::getWidth());
    float h = static_cast<float>(Window::getHeight());
    return glm::vec2((worldPosition.x / getWorldWidth() + 0.5f) * w, (0.5f - worldPosition.y / getWorldHeight()) * h);
}

glm::mat4 core::getProjectionMatrix() {
    float halfWidth = getWorldWidth() / 2.0f;
    float halfHeight = getWorldHeight() / 2.0f;
    return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
}

glm::mat4 core::getScaledProjectionMatrix() {
    float xScale = 1.0f;
    float yScale = 1.0f;
    float targetAspect = getWorldAspectRatio();
    float windowAspect = Window::getAspectRatio();
    if (windowAspect > targetAspect) {
        xScale = targetAspect / windowAspect;
    } else {
        yScale = windowAspect / targetAspect;
    }
    return glm::scale(getProjectionMatrix(), glm::vec3(xScale, yScale, 1.0f));
}

glm::vec2 core::lerp(glm::vec2 &&start, glm::vec2 &&finish, float alpha) {
    return glm::mix(start, finish, alpha);
}

float core::lerp(float start, float finish, float alpha) {
    return start + (finish - start) * alpha;
}
