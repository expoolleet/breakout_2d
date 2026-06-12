#include "game_level_generator.hpp"

#include "fast_random.hpp"

GameLevelGenerator::GameLevelGenerator(GameLevelGeneratorCreateInfo config) : m_config(config) {}

TileType GameLevelGenerator::getRandomTile() const {
    if (m_config.tileRegistry.empty()) return TileType::Default;

    float totalWeight = 0.0f;
    for (const auto &item : m_config.tileRegistry) {
        totalWeight += item.weight;
    }

    float randValue = fastrand::frandom() * totalWeight;

    float currentWeightSum = 0.0f;
    for (const auto &item : m_config.tileRegistry) {
        currentWeightSum += item.weight;
        if (randValue <= currentWeightSum) {
            return item.type;
        }
    }
    return m_config.tileRegistry.back().type;
}

PowerUpType GameLevelGenerator::getRandomPowerUp() const {
    if (m_config.powerUpRegistry.empty()) return PowerUpType::None;

    float totalWeight = 0.0f;
    for (const auto &item : m_config.powerUpRegistry) {
        totalWeight += item.weight;
    }

    float randValue = fastrand::frandom() * totalWeight;
    float currentWeightSum = 0.0f;
    for (const auto &item : m_config.powerUpRegistry) {
        currentWeightSum += item.weight;
        if (randValue <= currentWeightSum) {
            return item.type;
        }
    }
    return m_config.powerUpRegistry.back().type;
}

bool GameLevelGenerator::shouldSpawnTileAt(LevelShape shape, int x, int y, int width, int height) const {
    switch (shape) {
        case LevelShape::FullRectangle:
            return true;

        case LevelShape::Chessboard:
            return (x + y) % 2 == 0;

        case LevelShape::BordersOnly:
            return (x == 0 || x == width - 1 || y == 0 || y == height - 1);

        case LevelShape::Pyramid: {
            int midX = width / 2;
            int rowWidth = y + 1;
            return (x >= midX - rowWidth / 2) && (x <= midX + rowWidth / 2);
        }
    }
    return true;
}

GameLevel GameLevelGenerator::generate(int levelWidth, int levelHeight) {
    assert(levelWidth > 0 && levelHeight > 0);

    LevelTiles tiles(levelWidth, std::vector<int>(levelWidth, static_cast<int>(TileType::Empty)));
    std::vector<PowerUpType> powerUpTypes;

    for (int y = 0; y < levelHeight; ++y) {
        for (int x = 0; x < levelWidth; ++x) {
            if (!shouldSpawnTileAt(m_config.shape, x, y, levelWidth, levelHeight)) {
                continue;
            }

            TileType tile = getRandomTile();
            tiles[y][x] = static_cast<int>(tile);

            if (tile != TileType::Empty) {
                if (fastrand::frandom() < m_config.powerUpSpawnChance) {
                    powerUpTypes.push_back(getRandomPowerUp());
                } else {
                    powerUpTypes.push_back(PowerUpType::None);
                }
            } else {
                powerUpTypes.push_back(PowerUpType::None);
            }
        }
    }

    GameLevel level{m_config.levelCreateInfo, tiles};
    level.setPowerUps(powerUpTypes);

    return level;
}
