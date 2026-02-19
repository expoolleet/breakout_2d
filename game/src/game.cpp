#include "game.hpp"

#include "ball.hpp"
#include "collision_type.hpp"
#include "event_dispatcher.hpp"
#include "event_type.hpp"
#include "fast_random.hpp"
#include "game_level.hpp"
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

#include <GLFW/glfw3.h>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <thread>

glm::vec2 Game::_lerpPos(GameObject &gameObject, float alpha) {
    return glm::mix(gameObject.getPreviousPosition(), gameObject.getPosition(), alpha);
}

void Game::_calcBallNewPositionAndVelocity(Ball &ball, CollisionDirection dir, glm::vec2 diffVector) {
    glm::vec2 ballVelocity = ball.getVelocity();
    glm::vec2 ballPosition = ball.getPosition();
    if (dir == COLLISION_DIRECTION_LEFT || dir == COLLISION_DIRECTION_RIGHT) {
        ballVelocity.x = -ballVelocity.x;
        float shift = ball.getRadius() - std::abs(diffVector.x);
        if (dir == COLLISION_DIRECTION_LEFT) {
            ballPosition.x += shift;

        } else {
            ballPosition.x -= shift;
        }
    } else {
        ballVelocity.y = -ballVelocity.y;
        float shift = ball.getRadius() - std::abs(diffVector.y);
        if (dir == COLLISION_DIRECTION_UP) {
            ballPosition.y -= shift;
        } else {
            ballPosition.y += shift;
        }
    }
    ball.setVelocity(ballVelocity);
    ball.setPosition(ballPosition);
}

Game::Game(unsigned int width, unsigned int height, unsigned int attempts) : m_attempts(attempts), Width(width), Height(height) {
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
    glm::mat4 projectionMat = glm::ortho(0.0f, float(Window::getWidth()), 0.0f, float(Window::getHeight()));

    std::string shadersPath = PathManager::getResourcePath("shaders");
    m_spriteShader = std::make_shared<Shader>(shadersPath + "/vs/sprite.glsl", shadersPath + "/fs/sprite.glsl");
    m_spriteShader->setMat4("projection", projectionMat);
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
    m_levels.push_back(GameLevel(levelsPath + "/1.lvl", Window::getWidth() / 1.5, Window::getHeight() / 2));
    m_levels.push_back(GameLevel(levelsPath + "/2.lvl", Window::getWidth() / 1.5, Window::getHeight() / 2));
    m_levels.push_back(GameLevel(levelsPath + "/3.lvl", Window::getWidth() / 1.5, Window::getHeight() / 2));
    m_levels.push_back(GameLevel(levelsPath + "/4.lvl", Window::getWidth() / 1.5, Window::getHeight()));
    m_currentLevelNumber = 1;
    m_currentLevel = m_levels[m_currentLevelNumber - 1];
    m_currentLevel.load();

    m_currentLevel.getBricks()[0].setPowerUpType(PowerUpType::PowerUp_StickyPlayer);

    m_player = std::make_unique<Player>(TextureManager::getTexture("player"), PLAYER_START_POSITION, PLAYER_DEFAULT_SIZE);
    m_player->setSpeed(500.0f);

    auto ball = std::make_unique<Ball>(TextureManager::getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE, *m_player);
    ball->setVelocity(INITIAL_BALL_VELOCITY);
    ball->setRadius(ball->getSize().x / 2);
    ball->setSpeed(BALL_DEFAULT_SPEED);
    ball->setDamage(1);
    m_balls.push_back(std::move(ball));

    m_particleShader = std::make_shared<Shader>(shadersPath + "/vs/particle.glsl", shadersPath + "/fs/particle.glsl");
    m_particleShader->setMat4("projection", projectionMat);
    m_particleShader->setInt("sprite", 0);

    m_ballParticles = std::make_unique<ParticleEmitter>(TextureManager::getTexture("ball"), 1000);
    m_ballParticles->setPositionRandomOffsetRange(-5.0f, 5.0f);
    m_ballParticles->setVelocityRandomOffsetRange(-5.0f, 5.0f);
    m_ballParticles->setParticleAttenuationSpeed(2.5f);
    m_ballParticles->setParticleLifeTime(0.5f);
    m_ballParticles->setParticleScale(15.0f);
    m_ballParticles->setParticleDelay(0.9f);
    m_ballParticles->init();

    m_collisionHitParticles = std::make_unique<ParticleEmitter>(TextureManager::getTexture("ball"), 300);
    m_collisionHitParticles->setPositionRandomOffsetRange(-10.0f, 10.0f);
    m_collisionHitParticles->setVelocityRandomOffsetRange(-100.0f, 100.0f);
    m_collisionHitParticles->setParticleAttenuationSpeed(2.0f);
    m_collisionHitParticles->setParticleScale(10.0f);
    m_collisionHitParticles->setParticleLifeTime(1.0f);
    m_collisionHitParticles->setGravityEnabled(true);
    m_collisionHitParticles->setParticleColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_collisionHitParticles->init();

    EventDispatcher::Get().subscribe<BallFliedOff>([this](const BallFliedOff &e) { this->onBallFliedOff(e); });

    EventDispatcher::Get().subscribe<PowerUpActivated>([this](const PowerUpActivated &e) { this->onPowerUpActivated(e); });

    EventDispatcher::Get().subscribe<PowerUpFinished>([this](const PowerUpFinished &e) { this->onPowerUpFinished(e); });

    m_running = true;

#ifdef DEBUG_MODE
    unsigned int sleepTime = 250;
    m_consoleInputThread = std::thread([this, sleepTime]() {
        std::string command;
        while (m_running) {
            std::cin >> command;
            if (command == "ball") {
                this->spawnBall(glm::vec2(glm::vec2(Window::getWidth() / 2.0f, Window::getHeight() / 2.0f)));
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
    if (CurrentState != GAME_ACTIVE)
        return;

    glm::vec2 velocity = m_player->getVelocity();
    if (Keys[GLFW_KEY_A]) {
        velocity.x = -1.0f;
    } else if (Keys[GLFW_KEY_D]) {
        velocity.x = 1.0f;
    } else {
        velocity.x = 0.0f;
    }
    m_player->setVelocity(velocity);

    if (Keys[GLFW_KEY_SPACE]) {
        for (auto &ball : m_balls)
            ball->setStuck(false);
    }
}

void Game::update(float dt) {}

void Game::fixedUpdate(float dt) {
    if (CurrentState != GAME_ACTIVE)
        return;
    m_player->fixedUpdate(dt);
    int particlesPerFrame = 2;
    for (auto &ball : m_balls) {
        ball->fixedUpdate(dt);
        m_ballParticles->prepare(*ball, particlesPerFrame, glm::vec2(ball->getRadius() / 2));
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

    if (m_balls.size() == 1 && m_balls[0]->isDead()) {
        resetBalls();
    } else {
        cleanDestroyedBalls();
    }

    updatePowerUps(dt);
    cleanupPowerUps();
}

void Game::render(float alpha) {
    if (CurrentState == GAME_MENU)
        return;

    // objects
    m_spriteShader->use();
    m_spriteRenderer->drawSprite(*m_spriteShader, TextureManager::getTexture("background"), glm::vec2(0.0f, 0.0f),
                                 glm::vec2(float(Window::getWidth()), float(Window::getHeight())));
    for (const auto &brick : m_currentLevel.getBricks()) {
        if (!brick.isHidden()) {
            m_spriteRenderer->drawSprite(*m_spriteShader, *brick.Texture, brick.getPosition(), brick.getSize(), 0.0f, brick.Color);
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
        m_textRenderer->renderMSDF(*m_textShader, "YOU WON! :)", Window::getWidth() / 2 - 350, Window::getHeight() / 2, 3.0f,
                                   glm::vec3(0.0f, 0.9f, 0.6f), false, {glm::vec3(0.0f), 5.0f});
    } else {
        m_textRenderer->renderMSDF(*m_textShader, GAME_NAME, Window::getWidth() / 2 - 150, Window::getHeight() / 2, 1.0f, glm::vec3(1.0f),
                                   true, {glm::vec3(0.0f), 2.0f});
    }
    m_textRenderer->renderMSDF(*m_textShader, std::to_string(m_attempts - m_currentAttempt), 15, Window::getHeight() - 50);

    if (m_balls.size() > 0) {
        m_textRenderer->renderMSDF(
            *m_textShader, std::format("Ball velocity.x: {}; velocity.y: {}", m_balls[0]->getVelocity().x, m_balls[0]->getVelocity().y), 50,
            5, 0.5f, glm::vec3(1.0f), false, {glm::vec3(0.0f), 1.0f});
    }
}

void Game::nextLevel() {
    resetPlayer();
    resetBalls();
    m_currentAttempt = m_attempts;
    m_currentLevel = getLevel(++m_currentLevelNumber);
    m_currentLevel.load();
    CurrentState = GAME_ACTIVE;
}

GameLevel Game::getLevel(unsigned int number) {
    if (number > m_levels.size()) {
        _log::Warn("Could not return level with number {} because level count is {}. Returned first level.", number, m_levels.size());
        return m_levels[0];
    }
    return m_levels[number - 1];
}

void Game::restartCurrentLevel() {
    resetPlayer();
    resetBalls();
    m_currentLevel.restart();
    m_currentAttempt = 0;
}

void Game::resetBalls() {
    for (auto &ball : m_balls) {
        ball->reset();
        resetBallPosition(*ball);
    }
}

void Game::resetBallPosition(Ball &ball) {
    glm::vec2 ballPosition = m_player->getPosition() + glm::vec2(m_player->getSize().x / 2, ball.getSize().y);
    ball.resetPosition(ballPosition);
}

void Game::resetPlayer() {
    m_player->resetPosition(PLAYER_START_POSITION);
}

void Game::cleanDestroyedBalls() {
    std::erase_if(m_balls, [](const std::unique_ptr<Ball> &ball) { return ball->isDead(); });
}

void Game::spawnBall(glm::vec2 position) {
    auto ball = std::make_unique<Ball>(TextureManager::getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE, *m_player);
    ball->setPosition(position);
    glm::vec2 randomVelocity =
        glm::normalize(glm::vec2(_fr::randomFloatInRange(-1.0f, 1.0f), INITIAL_BALL_VELOCITY.y + _fr::randomFloatInRange(-0.5f, 0.5f)));
    ball->setVelocity(randomVelocity);
    ball->setRadius(ball->getSize().x / 2);
    float speedOffset = 50.0f;
    ball->setSpeed(_fr::randomFloatInRange(BALL_DEFAULT_SPEED - speedOffset, BALL_DEFAULT_SPEED + speedOffset));
    int damage = _fr::randomIntInRange(MIN_BALL_DAMAGE, MAX_BALL_DAMAGE);
    switch (damage) {
    case 1:
        ball->setColor(glm::vec3(1.0f));
        break; // default
    case 2:
        ball->setColor(glm::vec3(0.9f, 0.6f, 0.2f));
        break; // orange
    case 3:
        ball->setColor(glm::vec3(0.9f, 0.1f, 0.2f));
        break; // red
    }
    ball->setDamage(damage);
    ball->setStuck(false);
    m_balls.push_back(std::move(ball));
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
    m_powerups.push_back(_pf::createPowerUp(type, position));
}

Game::~Game() {
    if (m_consoleInputThread.joinable()) {
        m_running = false;
        m_consoleInputThread.join();
    }
}

void Game::onBallFliedOff(const BallFliedOff &e) {
    if (m_balls.size() == 1 && ++m_currentAttempt >= m_attempts) {
        this->restartCurrentLevel();
    } else {
        e.ball.destroy();
    }
}

void Game::onPowerUpActivated(const PowerUpActivated &e) {
    switch (e.type) {
    case PowerUp_FastBalls:
        break;
    case PowerUp_WidePlayer:
        break;
    case PowerUp_StickyPlayer:
        m_player->setStickyState(true);
        break;
    case PowerUp_PassTrough:
        break;
    default:
        break;
    }
}

void Game::onPowerUpFinished(const PowerUpFinished &e) {
    for (auto &powerup : m_powerups) {
        if (powerup.getType() == e.type && !powerup.isFinished()) {
            return;
        }
    }

    switch (e.type) {
    case PowerUp_FastBalls:
        break;
    case PowerUp_WidePlayer:
        break;
    case PowerUp_StickyPlayer:
        m_player->setStickyState(false);
        break;
    case PowerUp_PassTrough:
        break;
    default:
        break;
    }
}

void Game::doCollisions() {
    for (auto &poweup : m_powerups) {
        poweup.checkCollision(*m_player);
    }
    for (auto &ball : m_balls) {
        ball->checkCollision(*m_player);
    }
    for (auto &brick : m_currentLevel.getBricks()) {
        if (brick.isDead())
            continue;
        for (auto &ball : m_balls) {
            Collision collision = brick.checkCollision(*ball);
            if (std::get<0>(collision)) {
                CollisionDirection dir = std::get<1>(collision);
                glm::vec2 diffVector = std::get<2>(collision);
                glm::vec2 collisionPoint = std::get<3>(collision);
                m_collisionPointHistory.push_back(collisionPoint);
                _calcBallNewPositionAndVelocity(*ball, dir, diffVector);

                if (brick.getPowerUpType() != PowerUp_None) {
                    spawnPowerUp(brick.getPowerUpType(), collisionPoint);
                }

                if (m_currentLevel.isFinished()) {
                    CurrentState = GAME_WIN;
                }
            }
        }
    }
}
