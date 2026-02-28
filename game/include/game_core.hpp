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
    // return getWorldWidth() / Window::getAspectRatio();
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

inline glm::vec2 getScreenPosition(glm::vec2 worldPosition) {
    float w = static_cast<float>(Window::getWidth());
    float h = static_cast<float>(Window::getHeight());
    return glm::vec2((worldPosition.x / getWorldWidth() + 0.5f) * w, (0.5f - worldPosition.y / getWorldHeight()) * h);
}
};  // namespace core
