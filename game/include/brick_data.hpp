#pragma once

#include "brick_type.hpp"
#include "logging.hpp"

#include <format>
#include <glm/glm.hpp>

struct BrickData {
    glm::vec3 color;
    int maxHardnessPoints;
    bool isDestroyable;

    BrickData(glm::vec3 col, int points, bool flag) : color(col), maxHardnessPoints(points), isDestroyable(flag) {}
    BrickData() : color(glm::vec3(1.0f)), maxHardnessPoints(0), isDestroyable(false) {}
};

inline BrickData getBrickData(BrickType type) {
    switch (type) {
    case BrickType::None:
        return BrickData();
    case BrickType::Standard:
        return BrickData(glm::vec3(0.0f, 0.9f, 0.3f), 1, true);
    case BrickType::Hard:
        return BrickData(glm::vec3(0.9f, 0.7f, 0.2f), 2, true);
    case BrickType::ExtremelyTough:
        return BrickData(glm::vec3(0.9f, 0.2f, 0.0f), 3, true);
    case BrickType::Undestroyable:
        return BrickData(glm::vec3(1.0f, 1.0f, 1.0f), 0, false);
    default:
        logging::Warn("Specified wrong BrickType type: {} ({})", toString(type), int(type));
        return BrickData();
    }
}
