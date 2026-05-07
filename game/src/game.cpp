#include "game.hpp"

#include <GLFW/glfw3.h>

#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <thread>

#include "audio_events.hpp"
#include "audio_manager.hpp"
#include "ball.hpp"
#include "collision_type.hpp"
#include "event_dispatcher.hpp"
#include "event_type.hpp"
#include "fast_random.hpp"
#include "game_core.hpp"
#include "game_level.hpp"
#include "game_level_generator.hpp"
#include "game_object.hpp"
#include "logging.hpp"
#include "particle_emitter.hpp"
#include "path_manager.hpp"
#include "player.hpp"
#include "powerup.hpp"
#include "powerup_factory.hpp"
#include "powerup_type.hpp"
#include "shader.hpp"
#include "sprite_renderer.hpp"
#include "text_renderer.hpp"
#include "texture_manager.hpp"
#include "window.hpp"

glm::vec2 Game::_lerpPos(GameObject &gameObject, float alpha) {
    return glm::mix(gameObject.getPreviousPosition(), gameObject.getPosition(), alpha);
}

void Game::_calcBallNewPositionAndVelocity(Ball &ball, CollisionDirection dir, glm::vec2 diffVector) {
    glm::vec2 ballVelocity = ball.getVelocity();
    glm::vec2 ballPosition = ball.getPosition();
    if (dir == CollisionDirection::Left || dir == CollisionDirection::Right) {
        ballVelocity.x = -ballVelocity.x;
        float shift = ball.getRadius() - std::abs(diffVector.x);
        if (dir == CollisionDirection::Left) {
            ballPosition.x += shift;

        } else {
            ballPosition.x -= shift;
        }
    } else {
        ballVelocity.y = -ballVelocity.y;
        float shift = ball.getRadius() - std::abs(diffVector.y);
        if (dir == CollisionDirection::Up) {
            ballPosition.y -= shift;
        } else {
            ballPosition.y += shift;
        }
    }
    ball.setVelocity(ballVelocity);
    ball.setPosition(ballPosition);
}

Game::Game(unsigned int attempts) : m_attempts(attempts) {
    m_spriteShader = nullptr;
    m_textShader = nullptr;
    m_particleShader = nullptr;
    m_spriteRenderer = nullptr;
    m_textRenderer = nullptr;
    m_player = nullptr;
    m_ballParticles = nullptr;
    CurrentState = GAME_ACTIVE;
}

void Game::init() {
    m_maxCountBallsStuckToPlayer = 3;

    glm::mat4 view = glm::mat4(1.0f);
    // view = glm::translate(view, glm::vec3(100.0f, 100.0f, 0.0f));
    // view = glm::rotate(view, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    std::string shadersPath = PathManager::getResourcePath("shaders");
    m_spriteShader = std::make_shared<Shader>(shadersPath + "/vs/sprite.glsl", shadersPath + "/fs/sprite.glsl");
    m_spriteShader->setMat4("view", view);
    m_spriteShader->setInt("sprite", 0);
    m_spriteRenderer = std::make_unique<SpriteRenderer>();

    m_textShader = std::make_shared<Shader>(shadersPath + "/vs/text.glsl", shadersPath + "/fs/textMSDF.glsl");
    std::string fontsPath = PathManager::getResourcePath("fonts");
    m_textRenderer = std::make_unique<TextRenderer>(fontsPath.c_str());
    m_textRenderer->initRenderer();
    m_textRenderer->initFontMSDF(fontsPath + "/msdf/roboto/atlas.png", fontsPath + "/msdf/roboto/atlas.json");

    std::string texturesPath = PathManager::getResourcePath("textures");
    TextureManager::loadTexture(texturesPath + "/background_2.png", false, "background");
    TextureManager::loadTexture(texturesPath + "/brick_standard.png", true, STANDARD_BRICK);
    TextureManager::loadTexture(texturesPath + "/brick_undestroyable.png", true, UNDESTROYABLE_BRICK);
    TextureManager::loadTexture(texturesPath + "/ball.png", true, "ball");
    TextureManager::loadTexture(texturesPath + "/player_skin_2.png", true, "player");

    std::string levelsPath = PathManager::getResourcePath("levels");
    m_levels.push_back(GameLevel(levelsPath + "/1.lvl"));
    m_levels.push_back(GameLevel(levelsPath + "/2.lvl"));
    m_levels.push_back(GameLevel(levelsPath + "/3.lvl"));
    m_levels.push_back(GameLevel(levelsPath + "/4.lvl"));
    m_currentLevelNumber = 1;
    m_currentLevel = GameLevelGenerator::generate(10, 10);
    m_currentLevel.load();

    m_currentLevel.setBrickPowerUp(0, PowerUpType::StickyPlayer);
    m_currentLevel.setBrickPowerUp(1, PowerUpType::WidePlayer);
    m_currentLevel.setBrickPowerUp(2, PowerUpType::PassTrough);
    m_currentLevel.setBrickPowerUp(3, PowerUpType::FastBalls);

    m_player = std::make_unique<Player>(TextureManager::getTexture("player"), PLAYER_START_POSITION, PLAYER_DEFAULT_SIZE);
    m_player->setSpeed(30.0f);

    auto ball = std::make_unique<Ball>(TextureManager::getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE, *m_player);
    ball->setVelocity(INITIAL_BALL_VELOCITY);
    ball->setRadius(ball->getSize().x / 2);
    ball->setSpeed(BALL_DEFAULT_SPEED);
    ball->setDamage(1);
    m_heroBall = ball.get();
    m_balls.push_back(std::move(ball));
    stickBallToPlayer(*m_heroBall);

    m_particleShader = std::make_shared<Shader>(shadersPath + "/vs/particle.glsl", shadersPath + "/fs/particle.glsl");
    m_particleShader->setMat4("view", view);
    m_particleShader->setInt("sprite", 0);

    m_ballParticles = std::make_unique<ParticleEmitter>(TextureManager::getTexture("ball"), 1000);
    float ballRadius = m_heroBall->getRadius();
    m_ballParticles->setPositionRandomOffsetRange(-ballRadius / 2.0f, ballRadius / 2.0f);
    m_ballParticles->setVelocityRandomOffsetRange(-ballRadius / 2.0f, ballRadius / 2.0f);
    m_ballParticles->setParticleLifeTime(0.5f);
    m_ballParticles->setParticleScale(0.3f);
    m_ballParticles->setParticleDelay(0.9f);
    m_ballParticles->init();

    m_collisionHitParticles = std::make_unique<ParticleEmitter>(TextureManager::getTexture("ball"), 300);
    m_collisionHitParticles->setPositionRandomOffsetRange(-1.0f, 1.0f);
    m_collisionHitParticles->setVelocityRandomOffsetRange(-5.0f, 5.0f);
    m_collisionHitParticles->setParticleScale(0.2f);
    m_collisionHitParticles->setParticleLifeTime(1.0f);
    m_collisionHitParticles->setGravityEnabled(true);
    m_collisionHitParticles->setParticleColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_collisionHitParticles->init();

    setProjectionMatrix();

    EventDispatcher::Get().subscribe<BallFliedOff>([this](const BallFliedOff &e) { this->onBallFliedOff(e); });

    EventDispatcher::Get().subscribe<BallHit>([this](const BallHit &e) { this->onBallHit(e); });

    EventDispatcher::Get().subscribe<PowerUpActivated>([this](const PowerUpActivated &e) { this->onPowerUpActivated(e); });

    EventDispatcher::Get().subscribe<PowerUpFinished>([this](const PowerUpFinished &e) { this->onPowerUpFinished(e); });

    EventDispatcher::Get().subscribe<BallUnstuck>([this](const BallUnstuck &e) { this->onBallUnstuck(e); });

    EventDispatcher::Get().subscribe<BallStuck>([this](const BallStuck &e) { this->onBallStuck(e); });

    m_running = true;

#ifdef DEBUG_MODE
    unsigned int sleepTime = 250;
    m_consoleInputThread = std::thread([this, sleepTime]() {
        std::string command;
        while (m_running) {
            std::cin >> command;
            if (command == "ball") {
                this->spawnBall(glm::vec2(0.0f));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    });
#endif
}

void Game::processInput(float dt) {
    if (CurrentState == GAME_WIN) {
        if (Keys[GLFW_KEY_ENTER]) {
            nextLevel();
        }
    }

    glm::vec2 velocity = m_player->getVelocity();
    if (Keys[GLFW_KEY_A]) {
        velocity.x = -1.0f;
    } else if (Keys[GLFW_KEY_D]) {
        velocity.x = 1.0f;
    } else {
        velocity.x = 0.0f;
    }
    m_player->setVelocity(velocity);

    if (Keys[GLFW_KEY_SPACE] && m_stuckBalls.size() > 0) {
        unstickBallFromPlayer(*m_stuckBalls.back());
        Keys[GLFW_KEY_SPACE] = false;
    }
}

void Game::update(float dt) {}

void Game::fixedUpdate(float dt) {
    m_player->fixedUpdate(dt);

    if (!m_queueBalls.empty()) {
        for (auto &ball : m_queueBalls) {
            m_balls.push_back(std::move(ball));
        }
        m_queueBalls.clear();
    }

    int particlesPerFrame = 2;
    for (auto &ball : m_balls) {
        ball->fixedUpdate(dt);
        m_ballParticles->prepare(*ball, particlesPerFrame, glm::vec2(ball->getRadius() / 2), true);
    }
    m_ballParticles->update(dt);

    doCollisions();

    if (m_collisionPointHistory.size() > 0) {
        for (auto &pos : m_collisionPointHistory) {
            m_collisionHitParticles->prepareAtPosition(pos, 30);
        }
        m_collisionPointHistory.clear();
    }
    m_collisionHitParticles->update(dt);

    updatePowerUps(dt);
    cleanupPowerUps();
    cleanDestroyedBalls();
}

void Game::render(float alpha) {
    if (CurrentState == GAME_MENU) return;

    // objects
    m_spriteShader->use();
    m_spriteRenderer->drawSprite(*m_spriteShader, TextureManager::getTexture("background"),
                                 glm::vec2(core::getWorldAABB().x, core::getWorldAABB().y),
                                 glm::vec2(core::getWorldWidth(), core::getWorldHeight()));
    for (const auto &brick : m_currentLevel.getBricks()) {
        if (!brick.isHidden()) {
            m_spriteRenderer->drawSprite(*m_spriteShader, *brick.Texture, brick.getPosition(), brick.getSize(), 0.0f, brick.getColor());
        }
    }
    m_spriteRenderer->drawSprite(*m_spriteShader, *m_player->Texture, _lerpPos(*m_player, alpha), m_player->getSize());

    m_particleShader->use();
    m_collisionHitParticles->render(*m_particleShader);
    m_ballParticles->render(*m_particleShader);

    m_spriteShader->use();
    for (auto &ball : m_balls) {
        m_spriteRenderer->drawSprite(*m_spriteShader, *ball->Texture, _lerpPos(*ball, alpha), ball->getSize(), 0.0f, ball->getColor());
    }

    for (auto &powerup : m_powerups) {
        if (!powerup.isHidden()) {
            m_spriteRenderer->drawSprite(*m_spriteShader, *powerup.Texture, _lerpPos(powerup, alpha), powerup.getSize(), 0.0f,
                                         powerup.getColor());
        }
    }
}

void Game::renderText(float dt) {
    m_textShader->use();
    if (CurrentState == GAME_WIN) {
        glm::vec2 textPos = core::getWorldPosition(0.25f, 0.5f);
        m_textRenderer->renderMSDF(*m_textShader, "YOU WON! :)", textPos, 0.07f, glm::vec3(0.0f, 0.9f, 0.6f), false,
                                   {glm::vec3(0.0f), 5.0f});
    } else {
        glm::vec2 namePos = core::getWorldPosition(0.25f, 0.5f);
        m_textRenderer->renderMSDF(*m_textShader, GAME_NAME, namePos, 0.07f, glm::vec3(1.0f), true, {glm::vec3(0.0f), 2.0f});
    }
    m_textRenderer->renderMSDF(*m_textShader, std::format("Attempts: {}", std::to_string(m_attempts - m_currentAttempt)),
                               core::getWorldPosition(0.01f, 0.9f), 0.03f);
    m_textRenderer->renderMSDF(*m_textShader, std::format("Ball count: {}", m_balls.size()), core::getWorldPosition(0.85f, 0.01f), 0.02f,
                               glm::vec3(1.0f), false, {glm::vec3(0.0f), 1.0f});
    m_textRenderer->renderMSDF(*m_textShader, std::format("Hero ball speed: {:.2f}", m_heroBall->getSpeed()),
                               core::getWorldPosition(0.01f, 0.09f), 0.02f, glm::vec3(1.0f), false, {glm::vec3(0.0f), 1.0f});
    m_textRenderer->renderMSDF(
        *m_textShader, std::format("Hero ball vel.x: {:.2f}; vel.y: {:.2f}", m_heroBall->getVelocity().x, m_heroBall->getVelocity().y),
        core::getWorldPosition(0.01f, 0.05f), 0.02f, glm::vec3(1.0f), false, {glm::vec3(0.0f), 1.0f});
    m_textRenderer->renderMSDF(
        *m_textShader, std::format("Hero ball pos.x: {:.2f}; pos.y: {:.2f}", m_heroBall->getPosition().x, m_heroBall->getPosition().y),
        core::getWorldPosition(0.01f, 0.01f), 0.02f, glm::vec3(1.0f), false, {glm::vec3(0.0f), 1.0f});
}

void Game::nextLevel() {
    resetPlayer();
    resetHeroBall();
    clearStuckBallsExceptHeroBall();
    repositionStuckBallsOnPlayer();
    destroyBallsExceptHeroBall();
    m_currentAttempt = 0;
    m_currentLevel = getLevel(++m_currentLevelNumber);
    m_powerups.clear();
    m_currentLevel.load();
    CurrentState = GAME_ACTIVE;
}

GameLevel Game::getLevel(unsigned int number) {
    if (number > m_levels.size()) {
        logging::Warn("Could not return level with number {} because level count is {}. Returned first level.", number, m_levels.size());
        return m_levels[0];
    }
    return m_levels[number - 1];
}

void Game::restartCurrentLevel() {
    resetPlayer();
    resetHeroBall();
    destroyBallsExceptHeroBall();
    m_currentLevel.restart();
    m_currentAttempt = 0;
}

void Game::resetHeroBall() {
    m_heroBall->reset();
    m_heroBall->setStuck(true);
    m_heroBall->setSpeed(BALL_DEFAULT_SPEED);
    m_stuckBalls.push_back(m_heroBall);
    repositionStuckBallsOnPlayer();
}

void Game::resetBallPosition(Ball &ball) {
    glm::vec2 ballPosition = m_player->getPosition() + glm::vec2(m_player->getSize().x / 2.0f - ball.getSize().x / 2.0f, ball.getSize().y);
    ball.resetPosition(ballPosition);
}

void Game::resetPlayer() {
    m_player->setStickness(false);
    m_player->setSize(PLAYER_DEFAULT_SIZE);
    m_player->resetPosition(PLAYER_START_POSITION);
}

void Game::setProjectionMatrix() {
    glm::mat4 projectionMat = core::getScaledProjectionMatrix();
    m_spriteShader->setMat4("projection", projectionMat);
    m_particleShader->setMat4("projection", projectionMat);
    m_textRenderer->updateProjectionMat = true;
}

void Game::repositionStuckBallsOnPlayer() {
    float xPadding = 10.0f;
    float xOffset = static_cast<float>(m_stuckBalls.size()) * (m_heroBall->getSize().x / 2.0f);
    for (auto &ball : m_balls) {
        if (ball->isStuck()) {
            ball->setStuckLocalPosition(glm::vec2(m_player->getSize().x / 2.0f - xOffset, ball->getSize().y));
            xOffset -= ball->getSize().x + xPadding;
        }
    }
}

void Game::stickBallToPlayer(Ball &ball) {
    if (ball.isStuck()) return;

    bool isHero = (&ball == m_heroBall);
    size_t currentCount = m_stuckBalls.size();

    if (!isHero && currentCount == m_maxCountBallsStuckToPlayer - 1 && !m_heroBall->isStuck()) return;
    if (currentCount >= m_maxCountBallsStuckToPlayer) return;

    ball.setStuck(true);
    m_stuckBalls.push_back(&ball);
    repositionStuckBallsOnPlayer();
    EventDispatcher::Get().emit(BallStuck(ball));
}

void Game::unstickBallFromPlayer(Ball &ball) {
    auto it = std::find(m_stuckBalls.begin(), m_stuckBalls.end(), &ball);
    if (it == m_stuckBalls.end()) return;
    ball.setStuck(false);
    ball.setVelocity(INITIAL_BALL_VELOCITY);
    ball.setPosition(ball.getPosition() + ball.getVelocity() * 0.1f);
    m_stuckBalls.erase(it);
    repositionStuckBallsOnPlayer();
    EventDispatcher::Get().emit(BallUnstuck(ball));
}

void Game::cleanDestroyedBalls() {
    std::erase_if(m_balls, [this](const std::unique_ptr<Ball> &ball) { return ball->isDead(); });
}

void Game::destroyBallsExceptHeroBall() {
    for (auto &ball : m_balls) {
        if (&(*ball) != m_heroBall) {
            ball->destroy();
        }
    }
}

void Game::clearStuckBallsExceptHeroBall() {
    std::erase_if(m_stuckBalls, [this](Ball *ball) { return this->m_heroBall != ball; });
}

void Game::spawnBall(glm::vec2 position) {
    auto ball = std::make_unique<Ball>(TextureManager::getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE, *m_player);
    ball->setPosition(position);
    glm::vec2 randomVelocity = glm::normalize(
        glm::vec2(fastrand::randomFloatInRange(-1.0f, 1.0f), INITIAL_BALL_VELOCITY.y + fastrand::randomFloatInRange(-0.5f, 0.5f)));
    ball->setVelocity(randomVelocity);
    ball->setRadius(ball->getSize().x / 2);
    float speedOffset = 10.0f;
    ball->setSpeed(fastrand::randomFloatInRange(BALL_DEFAULT_SPEED - speedOffset, BALL_DEFAULT_SPEED + speedOffset));
    unsigned int damage = fastrand::randomUIntInRange(MIN_BALL_DAMAGE, MAX_BALL_DAMAGE);
    switch (damage) {
        case 1:
            ball->setColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
            break;  // default
        case 2:
            ball->setColor(glm::vec4(0.9f, 0.6f, 0.2f, 1.0f));
            break;  // orange
        case 3:
            ball->setColor(glm::vec4(0.9f, 0.1f, 0.2f, 1.0f));
            break;  // red
        default:
            logging::Warn("Could not apply color to the ball with given damage: {}", damage);
            break;
    }
    ball->setDamage(damage);
    ball->setStuck(false);
    m_queueBalls.push_back(std::move(ball));
}

void Game::updatePowerUps(float dt) {
    for (auto &powerup : m_powerups) {
        powerup.fixedUpdate(dt);
    }
}

void Game::cleanupPowerUps() {
    std::erase_if(m_powerups, [](PowerUp &powerup) { return powerup.isFinished(); });
}

void Game::spawnPowerUp(PowerUpType type, glm::vec2 position) {
    m_powerups.push_back(PowerUpFactory::createPowerUp(type, position));
}

std::vector<std::unique_ptr<Ball>> &Game::getBalls() {
    return m_balls;
}

Game::~Game() {
    if (m_consoleInputThread.joinable()) {
        m_running = false;
        m_consoleInputThread.join();
    }
}

void Game::onBallFliedOff(const BallFliedOff &e) {
    if (&e.ball == m_heroBall) {
        if (CurrentState == GAME_ACTIVE && ++m_currentAttempt >= m_attempts) {
            restartCurrentLevel();
        } else {
            resetHeroBall();
        }
        logging::LogSilent("Main ball");
    } else {
        e.ball.destroy();
    }
}

void Game::onPowerUpActivated(const PowerUpActivated &e) {
    switch (e.type) {
        case PowerUpType::FastBalls:
            for (auto &ball : m_balls) {
                ball->setSpeed(BALL_DEFAULT_SPEED * fastrand::randomFloatInRange(1.5f, 2.0f));
            }
            break;
        case PowerUpType::WidePlayer:
            m_player->setSize(glm::vec2(PLAYER_DEFAULT_SIZE.x * 1.5f, PLAYER_DEFAULT_SIZE.y));
            repositionStuckBallsOnPlayer();
            break;
        case PowerUpType::StickyPlayer:
            m_player->setStickness(true);
            break;
        case PowerUpType::PassTrough:
            for (auto &ball : m_balls) {
                ball->setColliding(false);
            }
            break;
        case PowerUpType::None:
        default:
            break;
    }
    AudioManager::Get().playOnce(AE_POWERUP_PICKUP);
}

void Game::onPowerUpFinished(const PowerUpFinished &e) {
    for (auto &powerup : m_powerups) {
        if (powerup.getType() == e.type && !powerup.isFinished()) {
            return;
        }
    }

    switch (e.type) {
        case PowerUpType::FastBalls:
            for (auto &ball : m_balls) {
                ball->setSpeed(BALL_DEFAULT_SPEED);
            }
            break;
        case PowerUpType::WidePlayer:
            m_player->setSize(PLAYER_DEFAULT_SIZE);
            repositionStuckBallsOnPlayer();
            break;
        case PowerUpType::StickyPlayer:
            m_player->setStickness(false);
            break;
        case PowerUpType::PassTrough:
            for (auto &ball : m_balls) {
                ball->setColliding(true);
            }
            break;
        case PowerUpType::None:
        default:
            break;
    }
}

void Game::onBallHit(const BallHit &e) {
    float panValue = (e.position.x / core::getWorldAABB().z);
    AudioManager::Get().changeGlobalParameter("Pan", panValue);
    if (e.collisionType == CollisionType::Player && m_player->isSticky()) {
        AudioManager::Get().changeGlobalParameter("Speed", 0.0f);
    } else {
        AudioManager::Get().changeGlobalParameter("Speed", e.ball.getSpeed());
    }
    switch (e.collisionType) {
        case CollisionType::Brick:
            AudioManager::Get().playOnce(AE_BALL_HIT_BRICK, e.position, e.ball.getVelocity() * e.ball.getSpeed());
            break;
        case CollisionType::Obstacle:
            AudioManager::Get().playOnce(AE_BALL_HIT_OBSTACLE, e.position, e.ball.getVelocity() * e.ball.getSpeed());
            break;
        case CollisionType::Player:
            AudioManager::Get().playOnce(AE_BALL_HIT_PLAYER, e.position, e.ball.getVelocity() * e.ball.getSpeed());
            break;
        case CollisionType::None:
        default:
            break;
    }
}

void Game::onBallUnstuck(const BallUnstuck &e) {
    AudioManager::Get().changeGlobalParameter("Speed", e.ball.getSpeed());
    AudioManager::Get().playOnce(AE_BALL_WHOOSH, e.ball.getPosition());
}

void Game::onBallStuck(const BallStuck &e) {}

void Game::doCollisions() {
    for (auto &poweup : m_powerups) {
        poweup.checkCollision(*m_player);
    }
    for (auto &ball : m_balls) {
        Collision collision = ball->checkCollision(*m_player);
        if (std::get<static_cast<int>(CollisionDataType::IsCollided)>(collision)) {
            EventDispatcher::Get().emit(
                BallHit{std::get<static_cast<int>(CollisionDataType::CollisionPoint)>(collision), *ball, CollisionType::Player});
            if (m_player->isSticky()) {
                stickBallToPlayer(*ball);
            }
        }
    }
    for (auto &brick : m_currentLevel.getBricks()) {
        if (brick.isDead()) continue;
        for (auto &ball : m_balls) {
            if (!ball->isColliding()) continue;
            Collision collision = brick.checkCollision(*ball);
            if (std::get<static_cast<int>(CollisionDataType::IsCollided)>(collision)) {
                CollisionDirection dir = std::get<static_cast<int>(CollisionDataType::CollisionDirection)>(collision);
                glm::vec2 diffVector = std::get<static_cast<int>(CollisionDataType::DifferenceVector)>(collision);
                glm::vec2 collisionPoint = std::get<static_cast<int>(CollisionDataType::CollisionPoint)>(collision);
                m_collisionPointHistory.push_back(collisionPoint);
                EventDispatcher::Get().emit(BallHit(collisionPoint, *ball, CollisionType::Brick));
                _calcBallNewPositionAndVelocity(*ball, dir, diffVector);

                if (brick.getPowerUpType() != PowerUpType::None) {
                    spawnPowerUp(brick.getPowerUpType(), brick.getPosition());
                }

                if (m_currentLevel.isFinished()) {
                    CurrentState = GAME_WIN;
                }
            }
        }
    }
}
