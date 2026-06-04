#pragma once

enum class PowerUpType {
    WidePlayer,
    StickyPlayer,
    PassTrough,
    FastHeroBall,
    None,
};

inline const char *toString(PowerUpType type) {
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
