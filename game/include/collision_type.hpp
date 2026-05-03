#pragma once

#include <glm/glm.hpp>
#include <tuple>

// Order is important
enum class CollisionDirection {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
    None
};

enum class CollisionType {
    Obstacle,
    Brick,
    Player,
    None
};

enum class CollisionDataType {
    IsCollided = 0,
    CollisionDirection = 1,
    DifferenceVector = 2,
    CollisionPoint = 3
};

// isCollided, CollisionDirection, diffVector, collisionPoint
using Collision = std::tuple<bool, CollisionDirection, glm::vec2, glm::vec2>;
