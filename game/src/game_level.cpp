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

GameLevel::GameLevel(const std::string &levelPath) {
    try {
        std::string line;
        std::ifstream fileStream{levelPath};

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
                m_tiles.push_back(row);
            }
        }
    } catch (std::ifstream::failure e) {
        logging::Error("{}", e.what());
    }
}

GameLevel::GameLevel(LevelTiles tiles) : m_tiles(tiles) {}

GameLevel::~GameLevel() noexcept {
    m_bricks.clear();
}

void GameLevel::load() {
    if (m_isLoaded) return;
    m_isLoaded = true;
    size_t rows = m_tiles.size();
    size_t columns = m_tiles[0].size();
    size_t brickCount = rows * columns;
    float offset = 0.1f;
    float xStart = -(static_cast<float>(columns) * BRICK_SIZE.x / 2.0f) - offset;
    float yStart = core::getWorldAABB().w - BRICK_SIZE.y;
    for (size_t i = 0; i < brickCount; ++i) {
        size_t row = i / columns;
        size_t col = i % columns;
        int code = m_tiles[row][col];
        if (code == 0) continue;  // air
        float xPos = xStart + BRICK_SIZE.x * static_cast<float>(col) + offset / 2.0F;
        float yPos = yStart - BRICK_SIZE.y * static_cast<float>(row) - offset;
        glm::vec2 position = glm::vec2(xPos + offset, yPos - offset);
        glm::vec2 size = BRICK_SIZE - offset;
        switch (code) {
            case -1:
                m_bricks.emplace_back(TextureManager::getTexture(UNDESTROYABLE_BRICK), position, size, BrickType::Undestroyable);
                break;
            case 1:
                m_bricks.emplace_back(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::Standard);
                break;
            case 2:
                m_bricks.emplace_back(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::Hard);
                break;
            case 3:
                m_bricks.emplace_back(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::ExtremelyTough);
                break;
            default:
                logging::Warn("Could not load a brick with the tile code: {}", code);
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

void GameLevel::setBricks(std::vector<Brick> bricks) {
    m_bricks = std::move(bricks);
}

const std::vector<Brick> &GameLevel::getBricks() const & noexcept {
    return m_bricks;
}

std::vector<Brick> &GameLevel::getBricks() & noexcept {
    return m_bricks;
}

bool GameLevel::isFinished() const noexcept {
    for (auto &brick : m_bricks) {
        if (!brick.isDestroyable()) continue;
        if (brick.getCurrentHardnessPoints() > 0) return false;
    }
    return true;
}

bool GameLevel::isLoaded() const noexcept {
    return m_isLoaded;
}

int GameLevel::getWidth() const noexcept {
    return m_width;
}

int GameLevel::getHeight() const noexcept {
    return m_height;
}

void GameLevel::setBrickPowerUp(size_t idx, PowerUpType type) {
    assert(idx >= 0 && idx < m_bricks.size());
    m_bricks[idx].setPowerUpType(type);
}
