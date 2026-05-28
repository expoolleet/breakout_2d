#pragma once

enum class PowerUpType {
    WidePlayer,
    StickyPlayer,
    PassTrough,
    FastBalls,
    None,
};

inline const char *toString(PowerUpType type) {
    switch (type) {
        case PowerUpType::FastBalls:
            return "FastBalls";
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
