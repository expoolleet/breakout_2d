#include "game_level_generator.hpp"

#include "fast_random.hpp"

GameLevel GameLevelGenerator::generate(int levelWidth, int levelHeight) {
    assert(levelWidth > 0 && levelHeight > 0);
    LevelTiles tiles;
    for (int i = 0; i < levelHeight; ++i) {
        std::vector<int> row;
        for (int j = 0; j < levelWidth; ++j) {
            float randValue = fastrand::random();

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
    return GameLevel{tiles};
}
