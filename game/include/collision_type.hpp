#pragma once

#include <glm/glm.hpp>
#include <tuple>

// Order is important
enum CollisionDirection {
    CollisionDirection_Up,
    CollisionDirection_Right,
    CollisionDirection_Down,
    CollisionDirection_Left,
    CollisionDirection_None
};

enum CollisionType {
    CollisionType_Obstacle,
    CollisionType_Brick,
    CollisionType_Player,
    CollisionType_None
};

// isCollided, CollisionDirection, diffVector, collisionPoint
typedef std::tuple<bool, CollisionDirection, glm::vec2, glm::vec2> Collision;
