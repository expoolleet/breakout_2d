#pragma once

#include <string>
#include <vector>

#include "brick.hpp"

using LevelTiles = std::vector<std::vector<int>>;

class GameLevel {
   private:
    std::vector<Brick> m_bricks = {};
    LevelTiles m_tiles = {};
    int m_width = 0;
    int m_height = 0;
    bool m_isLoaded = false;

   public:
    static GameLevel DefaultLevel;

    GameLevel() = default;
    GameLevel(const std::string &levelPath);
    GameLevel(LevelTiles tiles);
    ~GameLevel() noexcept;
    GameLevel &operator=(const GameLevel &) noexcept = default;
    GameLevel(const GameLevel &) noexcept = default;

    void load();
    void restart();
    bool isFinished() const noexcept;
    bool isLoaded() const noexcept;
    int getWidth() const noexcept;
    int getHeight() const noexcept;
    void setBrickPowerUp(size_t idx, PowerUpType type);
    void setBricks(std::vector<Brick> bricks);
    const std::vector<Brick> &getBricks() const & noexcept;
    std::vector<Brick> &getBricks() & noexcept;
};
