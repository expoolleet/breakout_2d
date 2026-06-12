#pragma once

enum class BrickType {
    Default,
    Medium,
    Hard,
    Undestroyable,
    None,
};

inline const char *toString(BrickType type) {
    switch (type) {
        case BrickType::Default:
            return "Default";
        case BrickType::Medium:
            return "Medium";
        case BrickType::Hard:
            return "Hard";
        case BrickType::Undestroyable:
            return "Undestroyable";
        case BrickType::None:
            return "None";
        default:
            return "";
    }
}
