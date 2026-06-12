#pragma once

#include <string>
#include <vector>

#include "brick.hpp"
#include "object_manager.hpp"
#include "powerup_factory.hpp"

enum class TileType : int {
    Empty = 0,
    Default = 1,
    Medium = 2,
    Hard = 3,
    Badrock = 4,
};

enum class LevelShape {
    FullRectangle,
    Pyramid,
    Chessboard,
    BordersOnly
};

template <typename T>
struct WeightedItem {
    T type;
    float weight;
};

using LevelTileRegisty = std::vector<WeightedItem<TileType>>;
using PowerRegistry = std::vector<WeightedItem<PowerUpType>>;
using LevelTiles = std::vector<std::vector<int>>;

struct GameLevelCreateInfo {
    ContextPtr contextPtr;
    ObjectManagerPtr objectManagerPtr;
    PowerUpFactoryPtr powerUpFactoryPtr;
};

class GameLevel : public Object2D {
   public:
    GameLevel() = default;
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
    void setRandomBrickPowerUp(PowerUpType type);
    void setPowerUps(std::vector<PowerUpType> powerUpTypes);
    void spawnPowerUp(PowerUpType type, glm::vec2 position);
    void updatePowerUps(float dt);
    void spawnBall(glm::vec2 position);
    void eraseFinishedPowerUps();
    void clearPowerUps();
    std::vector<BrickPtr> &getBricks() noexcept;
    std::vector<PowerUpPtr> &getPowerUps() noexcept;

    virtual void reset() noexcept override;

   private:
    LevelTiles m_tiles;
    std::vector<PowerUpType> m_powerUpTypes;
    std::vector<PowerUpPtr> m_powerUps;
    std::vector<BrickPtr> m_bricks;

    ObjectManagerPtr m_objectManager;
    PowerUpFactoryPtr m_powerUpFactory;

    int m_width = 0;
    int m_height = 0;
    bool m_loaded = false;
};
