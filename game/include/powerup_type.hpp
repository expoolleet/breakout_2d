#pragma once

enum class PowerUpType : int {
    None = 0,

    // Player
    WidePlayer,
    NarrowPlayer,
    StickyPlayer,
    SpeedPlayer,
    Laser,
    ExtraLife,

    // Ball
    PassTrough,
    SpeedBall,
    BallBox,

    // Others
    ScoreBonus,

    // Last type is for counting
    PowerUpTypeCount,
};

inline std::string to_string(PowerUpType type) {
    switch (type) {
        case PowerUpType::SpeedBall:
            return "SpeedBall";
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
