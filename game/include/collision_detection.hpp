#pragma once

#include <glm/glm.hpp>
#include <tuple>

#include "ball.hpp"
#include "collision_type.hpp"
#include "object_2d.hpp"

// Collision Detection namespace
namespace cd {
inline Collision NoneCollision = std::tuple(false, CollisionDirection::None, glm::vec2(0.0f), glm::vec2(0.0f));

inline CollisionDirection _getCollisionDirection(glm::vec2 target) {
    constexpr size_t directionCount = 4;
    static std::array<glm::vec2, directionCount> directions{
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, -1.0f),
        glm::vec2(-1.0f, 0.0f),
    };

    float maxContribution = 0.0f;
    unsigned int bestMatch = 0;
    for (unsigned int i = 0; i < directionCount; ++i) {
        float cosAngle = glm::dot(directions[i], glm::normalize(target));
        if (cosAngle > maxContribution) {
            maxContribution = cosAngle;
            bestMatch = i;
        }
    }
    return static_cast<CollisionDirection>(bestMatch);
}

// AABB
inline Collision checkCollision(Object2D &a, Object2D &b) {
    bool xCollision = a.getPosition().x + a.getSize().x >= b.getPosition().x && b.getPosition().x + b.getSize().x >= a.getPosition().x;
    bool yCollision = a.getPosition().y + a.getSize().y >= b.getPosition().y && b.getPosition().y + b.getSize().y >= a.getPosition().y;

    if (xCollision && yCollision) {
        AABB aabb_A = a.getAABB();
        AABB aabb_B = b.getAABB();

        glm::vec2 difference = aabb_A.center - aabb_B.center;
        glm::vec2 clamped = glm::clamp(difference, -aabb_B.halfSize, aabb_B.halfSize);
        glm::vec2 closestPoint = aabb_B.center + clamped;

        difference = closestPoint - aabb_A.center;

        return std::tuple(true, _getCollisionDirection(difference), difference, closestPoint);
    }
    return NoneCollision;
}

// AABB with circle object
inline Collision checkCollision(Ball &ball, Object2D &object) {
    AABB aabb = object.getAABB();

    glm::vec2 ballCenter = ball.getPosition() + ball.getRadius();

    glm::vec2 difference = ballCenter - aabb.center;

    glm::vec2 clamped = glm::clamp(difference, -aabb.halfSize, aabb.halfSize);

    glm::vec2 closestPoint = aabb.center + clamped;

    difference = closestPoint - ballCenter;
    if (glm::length(difference) < ball.getRadius()) {
        return std::tuple(true, _getCollisionDirection(difference), difference, closestPoint);
    }
    return NoneCollision;
}
}  // namespace cd
