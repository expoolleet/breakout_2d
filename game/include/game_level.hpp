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

class GameLevel : public Object2D {
   public:
    GameLevel() = default;
    GameLevel(GameLevelCreateInfo createInfo, const std::string &levelPath);
    GameLevel(GameLevelCreateInfo createInfo, LevelTiles tiles);
    virtual ~GameLevel() noexcept override;
    GameLevel &operator=(const GameLevel &) noexcept = default;
    GameLevel(const GameLevel &) noexcept = default;

    void load();
    bool isFinished() const noexcept;
    bool isLoaded() const noexcept;
    void cleanup();
    int getWidth() const noexcept;
    int getHeight() const noexcept;
    void setBrickPowerUp(size_t idx, PowerUpType type);
    void setBrickPowerUp(BrickPtr brick, PowerUpType type);
    void setRandomPowerUps(std::vector<PowerUpType> powerUpTypes);
    void setRandomBrickPowerUp(PowerUpType type);
    void spawnBall(glm::vec2 position);
    void spawnPowerUp(PowerUpType type, glm::vec2 position);
    void updatePowerUps(float dt);
    void eraseFinishedPowerUps();
    void clearPowerUps();
    std::vector<BrickPtr> &getBricks() noexcept;
    std::vector<PowerUpPtr> &getPowerUps() noexcept;

    virtual void reset() noexcept override;

   private:
    ObjectManagerPtr m_objectManager;
    PowerUpFactoryPtr m_powerUpFactory;
    std::vector<BrickPtr> m_bricks;
    std::vector<PowerUpType> m_powerUpTypes;
    std::vector<PowerUpPtr> m_powerUps;
    LevelTiles m_tiles;
    int m_width = 0;
    int m_height = 0;
    bool m_loaded = false;
};
