#include "game_level.hpp"

#include "brick.hpp"
#include "brick_type.hpp"
#include "logging.hpp"
#include "texture_manager.hpp"
#include "window.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

GameLevel::GameLevel(std::string levelPath, int width, int height, glm::vec2 offset) : m_width(width), m_height(height), m_offset(offset) {
    m_bricks.clear();
    try {
        std::string line;
        std::ifstream fileStream(levelPath);

        if (!fileStream.is_open()) {
            _log::Error("Could not load the level file: {}", levelPath);
            return;
        }
        int tileCode;
        if (fileStream) {
            while (std::getline(fileStream, line)) {
                std::istringstream stringStream(line);
                std::vector<int> row;
                while (stringStream >> tileCode) {
                    row.push_back(tileCode);
                }
                m_tileCodes.push_back(row);
            }
        }
    } catch (std::ifstream::failure e) {
        _log::Error("{}", e.what());
    }
}

GameLevel::~GameLevel() {
    m_bricks.clear();
}

void GameLevel::load() {
    if (m_isLoaded)
        return;
    m_isLoaded = true;

    int gridHeight = m_tileCodes.size();
    int gridWidth = m_tileCodes[0].size();

    float offset = 3.0f;
    float unitWidth = m_width / (static_cast<float>(gridWidth));
    float unitHeight = m_height / static_cast<float>(gridHeight);
    for (int i = 0; i < gridHeight; ++i) {
        for (int j = 0; j < gridWidth; ++j) {
            int code = m_tileCodes[i][j];
            if (code == 0)
                continue;

            // float xOffset = j * unitWidth + offset / 2;
            float xPos = (Window::getWidth() - m_width) / 2.0f + j * unitWidth + offset / 2;
            float yPos = Window::getHeight() - (unitHeight + i * unitHeight + offset);
            glm::vec2 position = glm::vec2(xPos, yPos) - m_offset;
            glm::vec2 size = glm::vec2(unitWidth - offset, unitHeight - offset);

            switch (code) {
            case -1:
                m_bricks.push_back(Brick(TextureManager::getTexture(UNDESTROYABLE_BRICK), position, size, BrickType::BRICK_UNDESTROYABLE));
                break;
            case 1:
                m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::BRICK_STANDARD));
                break;
            case 2:
                m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::BRICK_HARD));
                break;
            case 3:
                m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::BRICK_EXTREMELY_TOUGH));
                break;
            }
        }
    }
    std::partition(m_bricks.begin(), m_bricks.end(), [](auto &brick) { return brick.isDestroyable(); });
}

void GameLevel::restart() {
    for (auto &brick : m_bricks) {
        if (!brick.isDestroyable())
            continue;
        brick.reset();
    }
}

std::vector<Brick> &GameLevel::getBricks() {
    return m_bricks;
}

bool GameLevel::isFinished() {
    for (auto &brick : m_bricks) {
        if (!brick.isDestroyable())
            continue;
        if (brick.getCurrentHardnessPoints() > 0)
            return false;
    }
    return true;
}

bool GameLevel::isLoaded() {
    return m_isLoaded;
}
