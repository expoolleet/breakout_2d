#include "game_level.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "brick.hpp"
#include "brick_type.hpp"
#include "engine_context.hpp"
#include "fast_random.hpp"
#include "game_core.hpp"
#include "logging.hpp"
#include "powerup_factory.hpp"
#include "texture_literals.hpp"

using namespace texture_literals;

GameLevel::GameLevel(GameLevelCreateInfo createInfo, LevelTiles tiles)
    : m_ctx(createInfo.contextPtr),
      m_objectManager(createInfo.objectManagerPtr),
      m_powerupFactory(createInfo.powerUpFactoryPtr),
      m_tiles(std::move(tiles)) {}

GameLevel::GameLevel(GameLevelCreateInfo createInfo, const std::string &levelPath)
    : m_ctx(createInfo.contextPtr), m_objectManager(createInfo.objectManagerPtr), m_powerupFactory(createInfo.powerUpFactoryPtr) {
    std::string line;
    std::ifstream fileStream{levelPath};

    if (!fileStream.is_open()) {
        logging::Error("Could not load the level file: {}", levelPath);
        return;
    }
    int tileCode;
    if (fileStream) {
        while (std::getline(fileStream, line)) {
            std::istringstream stringStream{line};
            std::vector<int> row;
            while (stringStream >> tileCode) {
                row.push_back(tileCode);
            }
            m_tiles.push_back(row);
        }
    }
}

GameLevel::~GameLevel() noexcept {
    m_bricks.clear();
}

void GameLevel::load() {
    if (m_loaded) return;
    m_loaded = true;

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
                m_bricks.push_back(m_objectManager->create<Brick>(m_ctx->textureManager->getTexture(BADROCK_TEXTURE), position, size,
                                                                  BrickType::Undestroyable));
                break;
            case 1:
                m_bricks.push_back(
                    m_objectManager->create<Brick>(m_ctx->textureManager->getTexture(BRICK_TEXTURE), position, size, BrickType::Standard));
                break;
            case 2:
                m_bricks.push_back(
                    m_objectManager->create<Brick>(m_ctx->textureManager->getTexture(BRICK_TEXTURE), position, size, BrickType::Hard));
                break;
            case 3:
                m_bricks.push_back(m_objectManager->create<Brick>(m_ctx->textureManager->getTexture(BRICK_TEXTURE), position, size,
                                                                  BrickType::ExtremelyTough));
                break;
            default:
                logging::Warn("Could not load a brick with the tile code: {}", code);
                break;
        }
    }
    std::partition(m_bricks.begin(), m_bricks.end(), [](auto &brick) { return brick->isDestroyable(); });
}

void GameLevel::restart() {
    for (auto &brick : m_bricks) {
        if (!brick->isDestroyable()) continue;
        brick->reset();
    }
}

std::vector<BrickPtr> &GameLevel::getBricks() noexcept {
    return m_bricks;
}

bool GameLevel::isFinished() const noexcept {
    for (auto &brick : m_bricks) {
        if (!brick->isDestroyable()) continue;
        if (brick->getCurrentHardnessPoints() > 0) return false;
    }
    return true;
}

int GameLevel::getWidth() const noexcept {
    return m_width;
}

int GameLevel::getHeight() const noexcept {
    return m_height;
}

void GameLevel::setBrickPowerUp(size_t idx, PowerUpType type) {
    assert(idx >= 0 && idx < m_bricks.size());
    PowerUpData data = m_powerupFactory->getPowerUpData(type);
    m_bricks[idx]->setColor(data.color);
    m_bricks[idx]->setTexture(data.texture);
    m_bricks[idx]->setPowerUpType(type);
}

void GameLevel::setRandomBrickPowerUp(PowerUpType type) {
    setBrickPowerUp(fastrand::randomIntInRange(0, m_bricks.size() - 1), type);
}

bool GameLevel::isLoaded() const noexcept {
    return m_loaded;
}
