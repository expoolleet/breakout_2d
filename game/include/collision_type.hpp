#pragma once

#include <glm/glm.hpp>
#include <tuple>

enum CollisionDirection {
    COLLISION_DIRECTION_UP,
    COLLISION_DIRECTION_RIGHT,
    COLLISION_DIRECTION_DOWN,
    COLLISION_DIRECTION_LEFT,
    COLLISION_DIRECTION_NONE
};

typedef std::tuple<bool, CollisionDirection, glm::vec2, glm::vec2> Collision;
