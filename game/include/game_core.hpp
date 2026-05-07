#include <glm/glm.hpp>

#include "window.hpp"

namespace core {

constexpr float WORLD_WIDTH = 40.0f;
constexpr float WORLD_HEIGHT = 22.5f;  // 16:19 ratio

constexpr inline float getWorldWidth() {
    return WORLD_WIDTH;
}

inline float getWorldHeight() {
    return WORLD_HEIGHT;
}

inline float getWorldAspectRatio() {
    return getWorldWidth() / getWorldHeight();
}

inline glm::vec4 getWorldAABB() {
    float halfWidth = getWorldWidth() * 0.5f;
    float halfHeight = getWorldHeight() * 0.5f;
    return glm::vec4(-halfWidth, -halfHeight, halfWidth, halfHeight);
}

inline float getPixelsPerUnit() {
    return static_cast<float>(Window::getWidth()) / getWorldWidth();
}

inline glm::vec2 getWorldPosition(glm::vec2 screenPosition) {
    float w = static_cast<float>(Window::getWidth());
    float h = static_cast<float>(Window::getHeight());
    return glm::vec2((screenPosition.x / w - 0.5f) * getWorldWidth(), (0.5f - screenPosition.y / h) * getWorldHeight());
}

inline glm::vec2 getWorldPosition(float xNormalized, float yNormalized) {
    float halfWidth = getWorldWidth() / 2.0f;
    float halfHeight = getWorldHeight() / 2.0f;
    return glm::vec2((1.0f - xNormalized) * (-halfWidth) + xNormalized * halfWidth,
                     (1.0f - yNormalized) * (-halfHeight) + yNormalized * halfHeight);
}

inline glm::vec2 getScreenPosition(float xNormalized, float yNormalized) {
    return glm::vec2(xNormalized * static_cast<float>(Window::getWidth()), yNormalized * static_cast<float>(Window::getHeight()));
}

inline glm::vec2 getScreenPosition(glm::vec2 worldPosition) {
    float w = static_cast<float>(Window::getWidth());
    float h = static_cast<float>(Window::getHeight());
    return glm::vec2((worldPosition.x / getWorldWidth() + 0.5f) * w, (0.5f - worldPosition.y / getWorldHeight()) * h);
}

inline glm::mat4 getProjectionMatrix() {
    float halfWidth = core::getWorldWidth() / 2.0f;
    float halfHeight = core::getWorldHeight() / 2.0f;
    return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
}

inline glm::mat4 getScaledProjectionMatrix() {
    float xScale = 1.0f;
    float yScale = 1.0f;
    float targetAspect = core::getWorldAspectRatio();
    float windowAspect = Window::getAspectRatio();
    if (windowAspect > targetAspect) {
        xScale = targetAspect / windowAspect;
    } else {
        yScale = windowAspect / targetAspect;
    }
    return glm::scale(getProjectionMatrix(), glm::vec3(xScale, yScale, 1.0f));
}

};  // namespace core
