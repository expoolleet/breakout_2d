#include "game.hpp"

#include <cassert>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

#include "audio_events.hpp"
#include "ball.hpp"
#include "collision_type.hpp"
#include "engine_context.hpp"
#include "event_dispatcher.hpp"
#include "event_type.hpp"
#include "fast_random.hpp"
#include "game_core.hpp"
#include "game_level.hpp"
#include "game_level_generator.hpp"
#include "input.hpp"
#include "logging.hpp"
#include "object_manager.hpp"
#include "particle_emitter.hpp"
#include "player.hpp"
#include "powerup.hpp"
#include "powerup_factory.hpp"
#include "powerup_type.hpp"
#include "shader.hpp"
#include "text_renderer.hpp"
#include "texture_literals.hpp"
#include "texture_manager.hpp"
#include "timer.hpp"

using namespace texture_literals;
using namespace audio_events;

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

Game::Game(GameCreateInfo createInfo)
    : currentState(State::Active),
      m_mainScene(createInfo.contextPtr),
      m_spriteShader(createInfo.spriteShaderPtr),
      m_textShader(createInfo.textShaderPtr),
      m_particleShader(createInfo.particleShaderPtr),
      m_context(createInfo.contextPtr),
      m_objectManager(createInfo.objectManagerPtr),
      m_renderer(createInfo.gameRendererPtr),
      m_textRenderer(createInfo.textRendererPtr),
      m_ballParticleEmitter(createInfo.ballParticleEmitterPtr),
      m_collisionHitParticleEmitter(createInfo.collisionHitParticleEmitterPtr),
      m_background({m_context, m_context->getTextureManager().getTexture(BACKGROUND_TEXTURE)}),
      m_attempts(createInfo.gameAttempts) {
    assert(m_attempts > 0);
}

void Game::init() {
    m_maxCountBallsStuckToPlayer = 3;

    m_powerUpFactory = std::make_unique<PowerUpFactory>(PowerUpFactoryCreateInfo{m_context, m_objectManager});

    GameLevelCreateInfo levelCreateInfo{
        .contextPtr = m_context,
        .objectManagerPtr = m_objectManager,
        .powerUpFactoryPtr = m_powerUpFactory.get(),
    };

    m_levelGenerator = std::make_unique<GameLevelGenerator>(levelCreateInfo);

    std::string levelsPath = m_context->getPathManager().getResourcePath("levels");
    m_levels.push_back(GameLevel(levelCreateInfo, levelsPath + "/1.lvl"));
    m_levels.push_back(GameLevel(levelCreateInfo, levelsPath + "/2.lvl"));
    m_levels.push_back(GameLevel(levelCreateInfo, levelsPath + "/3.lvl"));
    m_levels.push_back(GameLevel(levelCreateInfo, levelsPath + "/4.lvl"));
    m_currentLevelNumber = 1;
    m_currentLevel = m_levelGenerator->generate(3, 3);
    m_currentLevel.load();
    m_currentLevel.setPosition(glm::vec2(0.0f, -7.0f));

    TextureManager &textureManager = m_context->getTextureManager();

    m_player = m_objectManager->create<Player>(textureManager.getTexture("player"), PLAYER_START_POSITION, PLAYER_DEFAULT_SIZE);
    m_player->setSpeed(30.0f);
    m_player->setAABB({glm::vec2(0.0f), (PLAYER_DEFAULT_SIZE / 2.0f) + 0.05f});
    m_currentLevel.addChild(m_player.get());

    BallPtr ball = m_objectManager->create<Ball>(textureManager.getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE);
    ball->setVelocity(INITIAL_BALL_VELOCITY);
    ball->setRadius(ball->getSize().x / 2);
    ball->setSpeed(BALL_DEFAULT_SPEED);
    ball->setDamage(1);
    m_heroBall = ball;
    m_balls.push_back(ball);
    stickBallToPlayer(m_heroBall);

    float ballRadius = m_heroBall->getRadius();
    m_ballParticleEmitter->setPositionRandomOffsetRange(-ballRadius / 2.0f, ballRadius / 2.0f);
    m_ballParticleEmitter->setVelocityRandomOffsetRange(-ballRadius / 2.0f, ballRadius / 2.0f);
    m_ballParticleEmitter->setParticleLifeTime(0.5f);
    m_ballParticleEmitter->setParticleScale(0.3f);
    m_ballParticleEmitter->setParticleDelay(0.9f);
    m_ballParticleEmitter->init();

    m_collisionHitParticleEmitter->setPositionRandomOffsetRange(-1.0f, 1.0f);
    m_collisionHitParticleEmitter->setVelocityRandomOffsetRange(-5.0f, 5.0f);
    m_collisionHitParticleEmitter->setParticleScale(0.2f);
    m_collisionHitParticleEmitter->setParticleLifeTime(1.0f);
    m_collisionHitParticleEmitter->setGravityEnabled(true);
    m_collisionHitParticleEmitter->setParticleColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_collisionHitParticleEmitter->init();

    setProjectionMatrix();

    m_renderer->setParticleShader(m_particleShader);
    m_renderer->addParticleEmitter(m_ballParticleEmitter);
    m_renderer->addParticleEmitter(m_collisionHitParticleEmitter);

    EventDispatcher &eventDispatcher = m_context->getEventDispatcher();
    eventDispatcher.subscribe<BallHit>([this](const BallHit &e) { _onBallHit(e); });
    eventDispatcher.subscribe<BallStuck>([this](const BallStuck &e) { _onBallStuck(e); });
    eventDispatcher.subscribe<BallUnstuck>([this](const BallUnstuck &e) { _onBallUnstuck(e); });
    eventDispatcher.subscribe<BallFliedOff>([this](const BallFliedOff &e) { _onBallFliedOff(e); });
    eventDispatcher.subscribe<GameFinished>([this](const GameFinished &e) { _onGameFinished(e); });
    eventDispatcher.subscribe<PowerUpFinished>([this](const PowerUpFinished &e) { _onPowerUpFinished(e); });
    eventDispatcher.subscribe<PowerUpActivated>([this](const PowerUpActivated &e) { _onPowerUpActivated(e); });
    eventDispatcher.subscribe<PowerUpSpawned>([this](const PowerUpSpawned &e) { spawnPowerUp(e.type, e.position); });
}

void Game::processInput(float dt) {
    if (currentState == State::Win && keys.justPressed(KEY_ENTER)) {
        currentState = State::Active;
        nextLevel();
    }

    if (currentState == State::Defeat && keys.justPressed(KEY_ENTER)) {
        currentState = State::Active;
        resetLevel(m_currentLevel);
    }

    if (currentState != State::Active) {
        return;
    }

    m_player->input(keys);

    if (m_stuckBalls.size() > 0 && keys.justPressed(KEY_SPACE)) {
        unstickBallFromPlayer(m_stuckBalls.back());
    }

    keys.update();
}

void Game::update(float dt) {
    _animateName(dt).resume();
    //_moveScene(dt).resume();
}

void Game::fixedUpdate(float dt) {
    m_player->fixedUpdate(dt);

    appendQueueBalls();

    updateBalls(dt);

    doCollisions();

    updateParticles(dt);

    updatePowerUps(dt);

    eraseFinishedPowerUps();
}

void Game::appendQueueBalls() {
    if (!m_queueBalls.empty()) {
        for (auto &ball : m_queueBalls) {
            m_balls.push_back(ball);
        }
        m_queueBalls.clear();
    }
}

void Game::updateParticles(float dt) {
    // Collision hits
    if (m_collisionPointHistory.size() > 0) {
        for (auto &pos : m_collisionPointHistory) {
            m_collisionHitParticleEmitter->prepareAtPosition(pos, m_collisionParticlesPerFrame);
        }
        m_collisionPointHistory.clear();
    }
    m_collisionHitParticleEmitter->update(dt);

    // Balls' trails
    for (auto &ball : m_balls) {
        if (!ball->isAlive()) continue;
        m_ballParticleEmitter->prepare(*ball, m_ballTrailParticlesPerFrame, glm::vec2(ball->getRadius() / 2), false);
    }
    m_ballParticleEmitter->update(dt);
}

void Game::updateBalls(float dt) {
    for (auto &ball : m_balls) {
        if (!ball->isAlive()) continue;
        ball->fixedUpdate(dt);
    }
}

void Game::render(float alpha) {
    if (currentState == State::Menu) return;

    if (!m_player->isHidden()) {
        m_renderer->submit({RenderLayer::Player, m_player->getShader(), m_player->getTexture(),
                            core::lerp(m_player->getPreviousPosition(), m_player->getPosition(), alpha), m_player->getSize()});
    }

    for (const auto &brick : m_currentLevel.getBricks()) {
        if (brick->isHidden()) continue;
        m_renderer->submit(
            {RenderLayer::Brick, brick->getShader(), brick->getTexture(), brick->getPosition(), brick->getSize(), 0.0f, brick->getColor()});
    }

    for (auto &ball : m_balls) {
        if (ball->isHidden()) continue;
        m_renderer->submit({RenderLayer::Ball, ball->getShader(), ball->getTexture(),
                            core::lerp(ball->getPreviousPosition(), ball->getPosition(), alpha), ball->getSize(), 0.0f, ball->getColor()});
    }

    for (auto &powerup : m_powerups) {
        if (powerup->isHidden()) continue;
        m_renderer->submit({RenderLayer::PowerUp, powerup->getShader(), powerup->getTexture(),
                            core::lerp(powerup->getPreviousPosition(), powerup->getPosition(), alpha), powerup->getSize(), 0.0f,
                            powerup->getColor()});
    }

    m_renderer->submit(
        {RenderLayer::Background, m_background.getShader(), m_background.getTexture(), m_background.getPosition(), m_background.getSize()});

    m_renderer->flush();
}

void Game::renderText(float dt) {
    m_textShader->use();

    if (currentState == State::Win) {
        m_textRenderer->renderMSDF(*m_textShader, "You won!\n(press Enter to continue)",
                                   {core::getWorldPosition(0.075f, 0.5f), glm::vec3(0.0f, 0.9f, 0.6f), 2.0f, TextAlign::Center},
                                   {.width = 2.0f});
    } else if (currentState == State::Defeat) {
        m_textRenderer->renderMSDF(*m_textShader, "Game over\n(press Enter to restart)",
                                   {core::getWorldPosition(0.075f, 0.5f), glm::vec3(0.9f, 0.1f, 0.3f), 2.0f, TextAlign::Center},
                                   {.width = 2.0f});
    } else {
        glm::vec2 namePos = core::getWorldPosition(0.3f, 0.3f);
        m_textRenderer->renderMSDF(*m_textShader, GAME_NAME, {namePos, glm::vec3(1.0f), m_nameSize, TextAlign::Left, true},
                                   {glm::vec3(0.0f), 1.0f});
    }
    m_textRenderer->renderMSDF(*m_textShader, std::format("Attempts: {}", std::to_string(m_attempts - m_currentAttempt)),
                               {.position = core::getWorldPosition(0.01f, 0.9f), .scale = 0.7f}, {glm::vec3(0.0f), 0.7f});

#ifdef DEBUG
    m_textRenderer->renderMSDF(*m_textShader, std::format("Ball (created) count: {}", m_balls.size()),
                               {core::getWorldPosition(0.65f, 0.01f), glm::vec3(1.0f), 0.7f}, {glm::vec3(0.0f), 0.7f});
    m_textRenderer->renderMSDF(*m_textShader, std::format("Hero ball speed: {:.2f}", m_heroBall->getSpeed()),
                               {core::getWorldPosition(0.01f, 0.09f), glm::vec3(1.0f), 0.8f}, {glm::vec3(0.0f), 0.8f});
    m_textRenderer->renderMSDF(
        *m_textShader, std::format("Hero ball vel.x: {:.2f}; vel.y: {:.2f}", m_heroBall->getVelocity().x, m_heroBall->getVelocity().y),
        {core::getWorldPosition(0.01f, 0.05f), glm::vec3(1.0f), 0.8f}, {glm::vec3(0.0f), 0.8f});
    m_textRenderer->renderMSDF(
        *m_textShader, std::format("Hero ball pos.x: {:.2f}; pos.y: {:.2f}", m_heroBall->getPosition().x, m_heroBall->getPosition().y),
        {core::getWorldPosition(0.01f, 0.01f), glm::vec3(1.0f), 0.8f}, {glm::vec3(0.0f), 0.8f});
    m_textRenderer->renderMSDF(*m_textShader, std::format("Object (created) count: {}", m_objectManager->size()),
                               {core::getWorldPosition(0.65f, 0.9f), glm::vec3(1.0f), 0.7f}, {glm::vec3(0.0f), 0.7f});
#endif
}

void Game::nextLevel() {
    clearBalls();
    keepStuckBalls();
    clearPowerUps();
    resetPlayer();
    resetHeroBall();
    repositionStuckBallsOnPlayer();
    m_currentLevel.cleanup();
    m_objectManager->cleanup();
    ++m_currentLevelNumber;
    m_currentLevel = m_levelGenerator->generate(5, 5);
    m_currentLevel.load();
    logging::Info("Next level is loaded");
}

void Game::resetLevel(GameLevel &level) {
    clearStuckBalls();
    clearBalls();
    clearPowerUps();
    resetPlayer();
    resetHeroBall();
    repositionStuckBallsOnPlayer();
    level.reset();
    m_objectManager->cleanup();
    m_currentAttempt = 0;
    logging::Info("Current level is reseted");
}

void Game::keepStuckBalls() {
    for (auto &ball : m_stuckBalls) {
        if (ball == m_heroBall) continue;
        ball->reset();
        m_balls.push_back(ball);
    }
}

void Game::resetHeroBall() {
    m_heroBall->reset();
    m_heroBall->setSpeed(BALL_DEFAULT_SPEED);
    m_heroBall->setStuck(true);
    m_heroBall->setParent(m_player.get());
    if (auto it = std::find(m_stuckBalls.begin(), m_stuckBalls.end(), m_heroBall); it == m_stuckBalls.end()) {
        m_stuckBalls.push_back(m_heroBall);
    }
}

void Game::resetBallPosition(BallPtr ball) {
    glm::vec2 ballPosition =
        m_player->getPosition() + glm::vec2(m_player->getSize().x / 2.0f - ball->getSize().x / 2.0f, ball->getSize().y);
    ball->resetPosition(ballPosition);
}

void Game::resetPlayer() {
    m_player->setStickness(false);
    m_player->setSize(PLAYER_DEFAULT_SIZE);
    m_player->resetPosition(PLAYER_START_POSITION);
}

GameLevel Game::getLevel(int lvlNumber) {
    if (lvlNumber < 1 || static_cast<size_t>(lvlNumber) > m_levels.size()) {
        logging::Warn("Could not return level with number {} because level count is {}. Returned first level.", lvlNumber, m_levels.size());
        return m_levels[0];
    }
    return m_levels[lvlNumber - 1];
}

void Game::setProjectionMatrix() {
    glm::mat4 projectionMat = core::getScaledProjectionMatrix();
    m_spriteShader->setMat4("projection", projectionMat);
    m_particleShader->setMat4("projection", projectionMat);
    m_textRenderer->updateProjectionMat = true;
}

void Game::repositionStuckBallsOnPlayer() {
    float xPadding = 0.02f;
    float xOffset = static_cast<float>(m_stuckBalls.size()) * (m_heroBall->getSize().x / 2.0f);
    for (auto &ball : m_stuckBalls) {
        ball->setLocalPosition(glm::vec2(m_player->getSize().x / 2.0f - xOffset, ball->getSize().y));
        xOffset -= ball->getSize().x + xPadding;
    }
}

void Game::stickBallToPlayer(BallPtr ball) {
    if (ball->isStuck()) return;

    bool isHero = (ball == m_heroBall);
    size_t stuckBallsCount = m_stuckBalls.size();

    if (!isHero && !m_heroBall->isStuck() && stuckBallsCount == m_maxCountBallsStuckToPlayer - 1) return;
    if (stuckBallsCount >= m_maxCountBallsStuckToPlayer) return;

    ball->setParent(m_player.get());
    ball->setStuck(true);
    m_stuckBalls.push_back(ball);
    repositionStuckBallsOnPlayer();
    m_context->getEventDispatcher().emit(BallStuck{ball});
}

void Game::unstickBallFromPlayer(BallPtr ball) {
    ball->setParent(nullptr);
    ball->setStuck(false);
    ball->setVelocity(INITIAL_BALL_VELOCITY);
    ball->setPosition(ball->getPosition() + ball->getVelocity() * 0.1f);
    m_stuckBalls.pop_back();
    repositionStuckBallsOnPlayer();
    m_context->getEventDispatcher().emit(BallUnstuck{ball});
}

void Game::clearBalls() {
    for (auto &ball : m_balls) {
        if (ball != m_heroBall) ball->destroy();
    }
    m_balls.clear();
    m_balls.push_back(m_heroBall);
}

void Game::clearStuckBalls() {
    for (auto &ball : m_stuckBalls) {
        ball->setStuck(false);
    }
    m_stuckBalls.clear();
    m_stuckBalls.push_back(m_heroBall);
}

void Game::spawnBall(glm::vec2 position) {
    auto ball = m_objectManager->create<Ball>(m_context->getTextureManager().getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE);
    ball->setPosition(position);
    glm::vec2 randomVelocity = glm::normalize(
        glm::vec2(fastrand::frandomFloatInRange(-1.0f, 1.0f), INITIAL_BALL_VELOCITY.y + fastrand::frandomFloatInRange(-0.5f, 0.5f)));
    ball->setVelocity(randomVelocity);
    ball->setRadius(ball->getSize().x / 2);
    float speedOffset = 10.0f;
    ball->setSpeed(fastrand::frandomFloatInRange(BALL_DEFAULT_SPEED - speedOffset, BALL_DEFAULT_SPEED + speedOffset));
    unsigned int damage = fastrand::randomUIntInRange(MIN_BALL_DAMAGE, MAX_BALL_DAMAGE);
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
    m_queueBalls.push_back(ball);
}

void Game::updatePowerUps(float dt) {
    for (auto &powerup : m_powerups) {
        powerup->fixedUpdate(dt);
    }
}

void Game::clearPowerUps() {
    for (auto &powerup : m_powerups) {
        powerup->destroy();
    }
    m_powerups.clear();
}

void Game::eraseFinishedPowerUps() {
    std::erase_if(m_powerups, [](PowerUpPtr powerup) { return powerup->isFinished(); });
}

void Game::spawnPowerUp(PowerUpType type, glm::vec2 position) {
    m_powerups.push_back(m_powerUpFactory->createPowerUp(type, position));
}

void Game::_onBallFliedOff(const BallFliedOff &e) {
    if (e.ball == m_heroBall) {
        if (!m_godMode && currentState == State::Active && ++m_currentAttempt >= m_attempts) {
            m_context->getEventDispatcher().emit(GameFinished{false});
        } else {
            stickBallToPlayer(m_heroBall);
        }
    } else {
        e.ball->destroy();
    }
}

void Game::_onPowerUpActivated(const PowerUpActivated &e) {
    switch (e.type) {
        case PowerUpType::FastHeroBall:
            m_heroBall->setSpeed(BALL_DEFAULT_SPEED * fastrand::randomFloatInRange(1.5f, 2.0f));
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
    m_context->getAudioManager().playOnce(POWERUP_PICKUP_SOUND);
}

void Game::_onPowerUpFinished(const PowerUpFinished &e) {
    for (auto &powerup : m_powerups) {
        if (powerup->getPowerUpType() == e.type && !powerup->isFinished()) {
            return;
        }
    }

    switch (e.type) {
        case PowerUpType::FastHeroBall:
            m_heroBall->setSpeed(BALL_DEFAULT_SPEED);
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

void Game::_onBallHit(const BallHit &e) {
    float panValue = (e.position.x / core::getWorldAABB().z);
    m_context->getAudioManager().changeGlobalParameter("Pan", panValue);
    if (e.collisionType == CollisionType::Player && m_player->isSticky()) {
        m_context->getAudioManager().changeGlobalParameter("Speed", 0.0f);
        return;
    } else {
        m_context->getAudioManager().changeGlobalParameter("Speed", e.ball->getSpeed());
    }
    switch (e.collisionType) {
        case CollisionType::Brick:
            m_context->getAudioManager().playOnce(BALL_HIT_BRICK_SOUND, e.position, e.ball->getVelocity() * e.ball->getSpeed());
            break;
        case CollisionType::Obstacle:
            m_context->getAudioManager().playOnce(BALL_HIT_OBSTACLE_SOUND, e.position, e.ball->getVelocity() * e.ball->getSpeed());
            break;
        case CollisionType::Player:
            m_context->getAudioManager().playOnce(BALL_HIT_PLAYER_SOUND, e.position, e.ball->getVelocity() * e.ball->getSpeed());
            break;
        case CollisionType::None:
        default:
            break;
    }
}

void Game::_onBallUnstuck(const BallUnstuck &e) {
    m_context->getAudioManager().changeGlobalParameter("Speed", e.ball->getSpeed());
    m_context->getAudioManager().playOnce(BALL_WHOOSH_SOUND, e.ball->getPosition());
}

void Game::_onBallStuck(const BallStuck &e) {
    m_context->getAudioManager().playOnce(BALL_STUCK_SOUND);
}

void Game::doCollisions() {
    for (auto &poweup : m_powerups) {
        if (!poweup->isAlive()) continue;
        poweup->checkCollision(*m_player);
    }
    for (auto &ball : m_balls) {
        if (!ball->isAlive()) continue;
        Collision collision = ball->checkCollision(*m_player);
        if (get<CollisionDataType::IsCollided>(collision)) {
            m_context->getEventDispatcher().emit(BallHit{ball, get<CollisionDataType::CollisionPoint>(collision), CollisionType::Player});
            if (m_player->isSticky()) {
                stickBallToPlayer(ball);
            }
        }
    }
    for (auto &brick : m_currentLevel.getBricks()) {
        if (!brick->isAlive()) continue;
        for (auto &ball : m_balls) {
            if (!brick->isAlive()) break;
            if (!ball->isColliding() || !ball->isAlive()) continue;
            Collision collision = brick->checkCollision(*ball);
            if (get<CollisionDataType::IsCollided>(collision)) {
                CollisionDirection dir = get<CollisionDataType::CollisionDirection>(collision);
                glm::vec2 diffVector = get<CollisionDataType::DifferenceVector>(collision);
                glm::vec2 collisionPoint = get<CollisionDataType::CollisionPoint>(collision);
                m_collisionPointHistory.push_back(collisionPoint);
                m_context->getEventDispatcher().emit(BallHit{ball, collisionPoint, CollisionType::Brick});
                _calcBallNewPositionAndVelocity(*ball, dir, diffVector);

                if (brick->getPowerUpType() != PowerUpType::None) {
                    m_context->getEventDispatcher().emit(PowerUpSpawned{brick->getPowerUpType(), brick->getPosition()});
                }

                if (m_currentLevel.isFinished()) {
                    m_context->getEventDispatcher().emit(GameFinished{true});
                }
            }
        }
    }
}

void Game::_onGameFinished(const GameFinished &e) {
    currentState = e.playerWon ? State::Win : State::Defeat;
}

Task Game::_animateName(float dt) {
    while (std::sin(timer::time()) > 0.0f) {
        m_nameSize += dt * 0.5f;
        co_await std::suspend_always{};
    }
    while (std::sin(timer::time()) < 0.0f) {
        m_nameSize -= dt * 0.5f;
        co_await std::suspend_always{};
    }
}

Task Game::_moveScene(float dt) {
    while (std::sin(timer::time()) > 0.0f) {
        m_currentLevel.setPosition(m_currentLevel.getPosition() + glm::vec2(0.0, dt));
        co_await std::suspend_always{};
    }
    while (std::sin(timer::time()) < 0.0f) {
        m_currentLevel.setPosition(m_currentLevel.getPosition() + glm::vec2(0.0, -dt));
        co_await std::suspend_always{};
    }
}

void Game::setGodMode(bool enabled) noexcept {
    m_godMode = enabled;
}

bool Game::isGodModeEnabled() const noexcept {
    return m_godMode;
}
