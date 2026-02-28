#pragma once

#include <glm/glm.hpp>

#include "brick_type.hpp"

struct BrickData {
    glm::vec4 color;
    int maxHardnessPoints;
    bool isDestroyable;

    constexpr BrickData(glm::vec4 col, int points, bool flag) : color(col), maxHardnessPoints(points), isDestroyable(flag) {}
    constexpr BrickData() : color(glm::vec4(1.0f)), maxHardnessPoints(0), isDestroyable(false) {}
};

constexpr BrickData getBrickData(BrickType type) {
    switch (type) {
        case BrickType::None:
            return BrickData();
        case BrickType::Standard:
            return BrickData(glm::vec4(0.0f, 0.9f, 0.3f, 1.0f), 1, true);
        case BrickType::Hard:
            return BrickData(glm::vec4(0.9f, 0.7f, 0.2f, 1.0f), 2, true);
        case BrickType::ExtremelyTough:
            return BrickData(glm::vec4(0.9f, 0.2f, 0.0f, 1.0f), 3, true);
        case BrickType::Undestroyable:
            return BrickData(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0, false);
        default:
            return BrickData();
    }
}
