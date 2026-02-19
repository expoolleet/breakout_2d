#pragma once

#include "brick_type.hpp"
#include "logging.hpp"

#include <format>
#include <glm/glm.hpp>

struct BrickData {
    glm::vec3 color;
    int maxHardnessPoints;
    bool isDestroyable;
};

inline BrickData getBrickData(BrickType type) {
    switch (type) {
    case BrickType::None:
        return {};
    case BrickType::Standard:
        return {.color = glm::vec3(0.0f, 0.9f, 0.3f), .maxHardnessPoints = 1, .isDestroyable = true};
    case BrickType::Hard:
        return {.color = glm::vec3(0.9f, 0.7f, 0.2f), .maxHardnessPoints = 2, .isDestroyable = true};
    case BrickType::ExtremelyTough:
        return {.color = glm::vec3(0.9f, 0.2f, 0.0f), .maxHardnessPoints = 3, .isDestroyable = true};
    case BrickType::Undestroyable:
        return {.color = glm::vec3(1.0f, 1.0f, 1.0f), .maxHardnessPoints = -1, .isDestroyable = false};
    default:
        _log::Warn("Specified wrong BrickType type: {} ({})", toString(type), int(type));
        return {};
    }
}
