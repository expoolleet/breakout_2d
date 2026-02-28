#include "game_level.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#include "brick.hpp"
#include "brick_type.hpp"
#include "game_core.hpp"
#include "logging.hpp"
#include "texture_manager.hpp"

GameLevel::GameLevel(std::string levelPath) {
    m_bricks.clear();
    try {
        std::string line;
        std::ifstream fileStream(levelPath);

        if (!fileStream.is_open()) {
            logging::Error("Could not load the level file: {}", levelPath);
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
        logging::Error("{}", e.what());
    }
}

GameLevel::~GameLevel() {
    m_bricks.clear();
}

void GameLevel::load() {
    if (m_isLoaded) return;
    m_isLoaded = true;

    size_t rows = m_tileCodes.size();
    size_t columns = m_tileCodes[0].size();
    size_t brickCount = rows * columns;
    float offset = 0.1f;
    float xStart = -(static_cast<float>(columns) * BRICK_SIZE.x / 2.0f) - offset;
    float yStart = core::getWorldAABB().w - BRICK_SIZE.y;
    for (size_t i = 0; i < brickCount; ++i) {
        size_t row = i / columns;
        size_t col = i % columns;
        int code = m_tileCodes[row][col];
        if (code == 0) continue;  // air
        float xPos = xStart + BRICK_SIZE.x * static_cast<float>(col) + offset / 2.0F;
        float yPos = yStart - BRICK_SIZE.y * static_cast<float>(row) - offset;
        glm::vec2 position = glm::vec2(xPos + offset, yPos - offset);
        glm::vec2 size = BRICK_SIZE - offset;
        switch (code) {
            case -1:
                m_bricks.push_back(Brick(TextureManager::getTexture(UNDESTROYABLE_BRICK), position, size, BrickType::Undestroyable));
                break;
            case 1:
                m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::Standard));
                break;
            case 2:
                m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::Hard));
                break;
            case 3:
                m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::ExtremelyTough));
                break;
            default:
                logging::Warn("Could not load a brick with code: {}", code);
                break;
        }
    }
    std::partition(m_bricks.begin(), m_bricks.end(), [](auto &brick) { return brick.isDestroyable(); });
}

void GameLevel::restart() {
    for (auto &brick : m_bricks) {
        if (!brick.isDestroyable()) continue;
        brick.reset();
    }
}

std::vector<Brick> &GameLevel::getBricks() {
    return m_bricks;
}

bool GameLevel::isFinished() {
    for (auto &brick : m_bricks) {
        if (!brick.isDestroyable()) continue;
        if (brick.getCurrentHardnessPoints() > 0) return false;
    }
    return true;
}

bool GameLevel::isLoaded() {
    return m_isLoaded;
}

int GameLevel::getWidth() const {
    return m_width;
}

int GameLevel::getHeight() const {
    return m_height;
}
