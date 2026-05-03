#pragma once

#include "game_level.hpp"

class GameLevelGenerator {
   public:
    /*
     * - levelWidth - level width in brick count
     * - levelHeight - level height in brick count
     */
    static GameLevel generate(int levelWidth, int levelHeight);
};
