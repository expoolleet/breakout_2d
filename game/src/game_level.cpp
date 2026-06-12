#include "game_level.hpp"

#include <algorithm>
#include <format>
#include <glm/glm.hpp>
#include <vector>

#include "ball.hpp"
#include "brick.hpp"
#include "brick_type.hpp"
#include "engine_context.hpp"
#include "event_type.hpp"
#include "fast_random.hpp"
#include "game_core.hpp"
#include "logging.hpp"
#include "powerup_factory.hpp"
#include "texture_literals.hpp"

using namespace texture_literals;

GameLevel::GameLevel(GameLevelCreateInfo createInfo, LevelTiles tiles)
    : Object2D(createInfo.contextPtr),
      m_tiles(std::move(tiles)),
      m_objectManager(createInfo.objectManagerPtr),
      m_powerUpFactory(createInfo.powerUpFactoryPtr) {}

GameLevel::~GameLevel() noexcept {
    m_bricks.clear();
}

void GameLevel::load() {
    if (m_loaded) return;
    m_loaded = true;

    TextureManager &textureManager = m_context->getTextureManager();

    size_t rows = m_tiles.size();
    size_t columns = m_tiles[0].size();
    size_t brickCount = rows * columns;

    std::vector<PowerUpType> powerUpTypes(brickCount, PowerUpType::None);
    copy(m_powerUpTypes.begin(), m_powerUpTypes.end(), powerUpTypes.begin());
    fastrand::shuffle(powerUpTypes);

    float offset = 0.1f;
    float xStart = -(static_cast<float>(columns) * BRICK_SIZE.x / 2.0f) - offset;
    float yStart = core::getWorldAABB().w - BRICK_SIZE.y;
    for (size_t i = 0; i < brickCount; ++i) {
        size_t row = i / columns;
        size_t col = i % columns;
        TileType code = static_cast<TileType>(m_tiles[row][col]);
        if (code == TileType::Empty) continue;
        float xPos = xStart + BRICK_SIZE.x * static_cast<float>(col) + offset / 2.0F;
        float yPos = yStart - BRICK_SIZE.y * static_cast<float>(row) - offset;
        glm::vec2 position = glm::vec2(xPos + offset, yPos - offset);
        glm::vec2 size = BRICK_SIZE - offset;

        switch (code) {
            case TileType::Badrock:
                m_bricks.push_back(
                    m_objectManager->create<Brick>(textureManager.getTexture(BADROCK_TEXTURE), position, size, BrickType::Undestroyable));
                break;
            case TileType::Default:
                m_bricks.push_back(
                    m_objectManager->create<Brick>(textureManager.getTexture(BRICK_TEXTURE), position, size, BrickType::Default));
                break;
            case TileType::Medium:
                m_bricks.push_back(
                    m_objectManager->create<Brick>(textureManager.getTexture(BRICK_TEXTURE), position, size, BrickType::Medium));
                break;
            case TileType::Hard:
                m_bricks.push_back(
                    m_objectManager->create<Brick>(textureManager.getTexture(BRICK_TEXTURE), position, size, BrickType::Hard));
                break;
            case TileType::Empty:
            default:
                logging::Warn("(GameLevel) Could not load a brick with the tile code: {}", static_cast<int>(code));
                break;
        }
        setBrickPowerUp(m_bricks.back(), powerUpTypes[i]);
    }

    for (auto &brick : m_bricks) {
        addChild(brick.get());
    }

    std::partition(m_bricks.begin(), m_bricks.end(), [](auto &brick) { return brick->isDestroyable(); });
}

void GameLevel::reset() noexcept {
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
    if (type == PowerUpType::None) return;
    PowerUpData data = m_powerUpFactory->getPowerUpData(type);
    m_bricks[idx]->setHardenessPoints(1);
    m_bricks[idx]->setColor(data.color);
    m_bricks[idx]->setTexture(data.texture);
    m_bricks[idx]->setPowerUpType(type);
}

void GameLevel::setBrickPowerUp(BrickPtr brick, PowerUpType type) {
    if (type == PowerUpType::None) return;
    PowerUpData data = m_powerUpFactory->getPowerUpData(type);
    brick->setHardenessPoints(1);
    brick->setColor(data.color);
    brick->setTexture(data.texture);
    brick->setPowerUpType(type);
}

void GameLevel::setPowerUps(std::vector<PowerUpType> powerUpTypes) {
    m_powerUpTypes = std::move(powerUpTypes);
}

void GameLevel::setRandomBrickPowerUp(PowerUpType type) {
    setBrickPowerUp(fastrand::randomIntInRange(0, m_bricks.size() - 1), type);
}

bool GameLevel::isLoaded() const noexcept {
    return m_loaded;
}

void GameLevel::cleanup() {
    for (auto &brick : m_bricks) {
        brick->destroy();
    }
}

void GameLevel::spawnBall(glm::vec2 position) {
    auto ball = m_objectManager->create<Ball>(m_context->getTextureManager().getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE);
    ball->setPosition(position);
    glm::vec2 randomVelocity = glm::normalize(
        glm::vec2(fastrand::frandomFloatInRange(-1.0f, 1.0f), BALL_INITIAL_VELOCITY.y + fastrand::frandomFloatInRange(-0.5f, 0.5f)));
    ball->setVelocity(randomVelocity);
    ball->setRadius(ball->getSize().x / 2);
    float speedOffset = 10.0f;
    ball->setSpeed(fastrand::frandomFloatInRange(BALL_DEFAULT_SPEED - speedOffset, BALL_DEFAULT_SPEED + speedOffset));
    int damage = fastrand::randomIntInRange(BALL_DAMAGE_MIN, BALL_DAMAGE_MAX);
    switch (damage) {
        case 1:
            ball->setColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
            break;
        case 2:
            ball->setColor(glm::vec4(0.9f, 0.6f, 0.2f, 1.0f));
            break;
        case 3:
            ball->setColor(glm::vec4(0.9f, 0.1f, 0.2f, 1.0f));
            break;
        default:
            logging::Warn("Could not apply color to the ball with given damage: {}", damage);
            break;
    }
    ball->setDamage(damage);
    ball->setStuck(false);
    ball->setParent(this);
    m_context->getEventDispatcher().emit(BallSpawned{ball});
}

void GameLevel::updatePowerUps(float dt) {
    for (auto &powerUp : m_powerUps) {
        powerUp->fixedUpdate(dt);
    }
}

void GameLevel::clearPowerUps() {
    for (auto &powerUp : m_powerUps) {
        powerUp->destroy();
    }
    m_powerUps.clear();
}

void GameLevel::eraseFinishedPowerUps() {
    std::erase_if(m_powerUps, [](PowerUpPtr powerUp) { return powerUp->isFinished(); });
}

void GameLevel::spawnPowerUp(PowerUpType type, glm::vec2 position) {
    m_powerUps.push_back(m_powerUpFactory->createPowerUp(type, position));
    m_powerUps.back()->setParent(this);
    m_context->getEventDispatcher().emit(PowerUpSpawned{type, position});
}

std::vector<PowerUpPtr> &GameLevel::getPowerUps() noexcept {
    return m_powerUps;
}
