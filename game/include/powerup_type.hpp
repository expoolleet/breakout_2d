#pragma once

enum class PowerUpType {
    None = 0,
    WidePlayer = 1,
    StickyPlayer = 2,
    PassTrough = 3,
    FastHeroBall = 4,
};

inline std::string to_string(PowerUpType type) {
    switch (type) {
        case PowerUpType::FastHeroBall:
            return "FastHeroBall";
        case PowerUpType::StickyPlayer:
            return "StickyPlayer";
        case PowerUpType::PassTrough:
            return "PassTrough";
        case PowerUpType::WidePlayer:
            return "WidePlayer";
        case PowerUpType::None:
            return "None";
        default:
            return "Unknown";
    }
}
