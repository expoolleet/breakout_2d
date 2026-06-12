#pragma once

#include "game_level.hpp"

struct GameLevelGeneratorCreateInfo {
    GameLevelCreateInfo levelCreateInfo;

    LevelTileRegisty tileRegistry;
    PowerRegistry powerUpRegistry;
    LevelShape shape = LevelShape::FullRectangle;

    float powerUpSpawnChance = 0.2f;
};

class GameLevelGenerator {
   private:
    GameLevelGeneratorCreateInfo m_config;

   public:
    GameLevelGenerator(GameLevelGeneratorCreateInfo config);
    /*
     * - levelWidth - level width in brick count
     * - levelHeight - level height in brick count
     */
    TileType getRandomTile() const;
    PowerUpType getRandomPowerUp() const;
    bool shouldSpawnTileAt(LevelShape shape, int x, int y, int width, int height) const;
    GameLevel generate(int levelWidth, int levelHeight);
};
