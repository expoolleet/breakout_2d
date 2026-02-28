#pragma once

#include <string>
#include <vector>

#include "brick.hpp"

class GameLevel {
   private:
    std::vector<Brick> m_bricks = {};
    std::vector<std::vector<int>> m_tileCodes = {};
    int m_width = 0;
    int m_height = 0;
    bool m_isLoaded = false;

   public:
    static GameLevel DefaultLevel;

    GameLevel() = default;
    GameLevel(std::string levelPath);
    ~GameLevel();

    void load();
    void restart();
    std::vector<Brick> &getBricks();
    bool isFinished();
    bool isLoaded();
    int getWidth() const;
    int getHeight() const;
};
