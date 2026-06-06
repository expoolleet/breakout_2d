#pragma once

#include <string>
#include <vector>

#include "brick.hpp"
#include "object_manager.hpp"
#include "powerup_factory.hpp"

using LevelTiles = std::vector<std::vector<int>>;

struct GameLevelCreateInfo {
    ContextPtr contextPtr;
    ObjectManagerPtr objectManagerPtr;
    PowerUpFactoryPtr powerUpFactoryPtr;
};

class GameLevel {
   public:
    GameLevel() = default;
    GameLevel(GameLevelCreateInfo createInfo, const std::string &levelPath);
    GameLevel(GameLevelCreateInfo createInfo, LevelTiles tiles);
    ~GameLevel() noexcept;
    GameLevel &operator=(const GameLevel &) noexcept = default;
    GameLevel(const GameLevel &) noexcept = default;

    void load();
    void reset();
    bool isFinished() const noexcept;
    bool isLoaded() const noexcept;
    void cleanup();
    int getWidth() const noexcept;
    int getHeight() const noexcept;
    void setBrickPowerUp(size_t idx, PowerUpType type);
    void setBrickPowerUp(BrickPtr brick, PowerUpType type);
    void setRandomPowerUps(std::vector<PowerUpType> powerUpTypes);
    void setRandomBrickPowerUp(PowerUpType type);
    std::vector<BrickPtr> &getBricks() noexcept;

   private:
    ContextPtr m_context;
    ObjectManagerPtr m_objectManager;
    PowerUpFactoryPtr m_powerupFactory;
    std::vector<BrickPtr> m_bricks;
    std::vector<PowerUpType> m_powerUpTypes;
    LevelTiles m_tiles;
    int m_width = 0;
    int m_height = 0;
    bool m_loaded = false;
};
