#pragma once

#include "game_level.hpp"

class GameLevelGenerator {
   private:
    GameLevelCreateInfo m_levelCreateInfo;

   public:
    GameLevelGenerator(GameLevelCreateInfo createInfo);
    /*
     * - levelWidth - level width in brick count
     * - levelHeight - level height in brick count
     */
    GameLevel generate(int levelWidth, int levelHeight);
};
