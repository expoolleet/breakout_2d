#include "game_level_generator.hpp"

#include "fast_random.hpp"

GameLevelGenerator::GameLevelGenerator(GameLevelCreateInfo createInfo) : m_levelCreateInfo(createInfo) {}

GameLevel GameLevelGenerator::generate(int levelWidth, int levelHeight) {
    assert(levelWidth > 0 && levelHeight > 0);
    LevelTiles tiles;
    for (int i = 0; i < levelHeight; ++i) {
        std::vector<int> row;
        for (int j = 0; j < levelWidth; ++j) {
            float randValue = fastrand::frandom();

            if (randValue < 0.7f) {
                row.push_back(1);
            } else if (randValue < 0.95f) {
                row.push_back(2);
            } else {
                row.push_back(3);
            }
        }
        tiles.push_back(row);
    }
    GameLevel level{m_levelCreateInfo, tiles};

    std::vector<PowerUpType> powerUpTypes;
    int powerUpMaxCount = levelWidth * levelHeight / 3;
    for (int i = 0; i < powerUpMaxCount; ++i) {
        powerUpTypes.push_back(static_cast<PowerUpType>(fastrand::frandomIntInRange(0, 4)));
    }
    level.setRandomPowerUps(powerUpTypes);
    return level;
}
