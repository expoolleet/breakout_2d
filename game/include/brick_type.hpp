#pragma once

enum BrickType {
    None,
    Standard,
    Hard,
    ExtremelyTough,
    Undestroyable,
};

inline const char *toString(BrickType type) {
    switch (type) {
    case Standard:
        return "Brick_Standard";
    case Hard:
        return "Brick_Hard";
    case ExtremelyTough:
        return "Brick_ExtremelyTough";
    case Undestroyable:
        return "Brick_Undestroyable";
    default:
        return "Brick_None";
    }
}
