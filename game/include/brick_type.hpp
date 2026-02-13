#pragma once

enum BrickType {
    BRICK_NONE,
    BRICK_STANDARD,
    BRICK_HARD,
    BRICK_EXTREMELY_TOUGH,
    BRICK_UNDESTROYABLE
};

inline const char *toString(BrickType type) {
    switch (type) {
    case BrickType::BRICK_NONE:
        return "BRICK_NONE";
    case BrickType::BRICK_STANDARD:
        return "BRICK_STANDARD";
    case BrickType::BRICK_HARD:
        return "BRICK_HARD";
    case BrickType::BRICK_EXTREMELY_TOUGH:
        return "BRICK_EXTREMELY_TOUGH";
    case BrickType::BRICK_UNDESTROYABLE:
        return "BRICK_UNDESTROYABLE";
    default:
        return "BRICK_UNKNOWN";
    }
}
