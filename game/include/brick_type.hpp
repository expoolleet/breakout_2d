#pragma once

enum class BrickType {
    Standard,
    Hard,
    ExtremelyTough,
    Undestroyable,
    None,
};

inline const char *toString(BrickType type) {
    switch (type) {
        case BrickType::Standard:
            return "Standard";
        case BrickType::Hard:
            return "Hard";
        case BrickType::ExtremelyTough:
            return "ExtremelyTough";
        case BrickType::Undestroyable:
            return "Undestroyable";
        case BrickType::None:
            return "None";
        default:
            return "";
    }
}
