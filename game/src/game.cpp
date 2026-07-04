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
#include "tween.hpp"

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

    GameLevelGeneratorCreateInfo generatorCreateInfo{
        .levelCreateInfo = levelCreateInfo,
        .tileRegistry = {{TileType::Default, 70.0f}, {TileType::Hard, 25.0f}, {TileType::Badrock, 5.0f}},
        .powerUpRegistry = {{PowerUpType::SpeedBall, 100.0f}},
        .shape = LevelShape::BordersOnly,
    };

    m_levelGenerator = std::make_unique<GameLevelGenerator>(generatorCreateInfo);

    m_currentLevelNumber = 1;
    currentLevel = m_levelGenerator->generate(3, 3);
    currentLevel.load();
    currentLevel.setPosition(glm::vec2(0.0f, -7.0f));

    TextureManager &textureManager = m_context->getTextureManager();

    m_player = m_objectManager->create<Player>(textureManager.getTexture("player"), PLAYER_START_POSITION, PLAYER_DEFAULT_SIZE);
    m_player->setSpeed(30.0f);
    m_player->setAABB({glm::vec2(0.0f), (PLAYER_DEFAULT_SIZE / 2.0f) + 0.05f});
    currentLevel.addChild(m_player.get());

    BallPtr ball = m_objectManager->create<Ball>(textureManager.getTexture("ball"), glm::vec2(0.0f), BALL_DEFAULT_SIZE);
    ball->setVelocity(BALL_INITIAL_VELOCITY);
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

    EventDispatcher &ed = m_context->getEventDispatcher();
    ed.subscribe<BallHit>([this](const BallHit &e) { _onBallHit(e); });
    ed.subscribe<BallStuck>([this](const BallStuck &e) { _onBallStuck(e); });
    ed.subscribe<BallUnstuck>([this](const BallUnstuck &e) { _onBallUnstuck(e); });
    ed.subscribe<BallSpawned>([this](const BallSpawned &e) { m_queueBalls.push_back(e.ball); });
    ed.subscribe<BallFliedOff>([this](const BallFliedOff &e) { _onBallFliedOff(e); });
    ed.subscribe<GameFinished>([this](const GameFinished &e) { _onGameFinished(e); });
    ed.subscribe<PowerUpSpawned>([this](const PowerUpSpawned &e) { _onPowerUpSpawned(e); });
    ed.subscribe<PowerUpFinished>([this](const PowerUpFinished &e) { _onPowerUpFinished(e); });
    ed.subscribe<PowerUpActivated>([this](const PowerUpActivated &e) { _onPowerUpActivated(e); });

    TweenPtr tween = Tween::createTween();
    tween->tweenProperty([&](TweenValue value) { m_player->setColor(std::get<glm::vec4>(value)); }, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                         glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), 1.0f);
    tween->tweenProperty([&](TweenValue value) { m_nameSize = std::get<float>(value); }, 0.5f, 3.0f, 4.0f);
    tween->loop();
    tween->reverse();
}

void Game::update(float delta) {
    Tween::updateAll(delta);
}

void Game::processInput(float delta) {
    if (currentState == State::Win && keys.justPressed(KEY_ENTER)) {
        currentState = State::Active;
        nextLevel();
    }

    if (currentState == State::Defeat && keys.justPressed(KEY_ENTER)) {
        currentState = State::Active;
        resetLevel(currentLevel);
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

void Game::fixedUpdate(float delta) {
    appendQueueBalls();

    updateGameObjects(delta);

    doCollisions();

    updateParticles(delta);

    currentLevel.updatePowerUps(delta);
    currentLevel.eraseFinishedPowerUps();
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

void Game::updateGameObjects(float delta) {
    m_gameObjects.clear();
    m_gameObjects.push_back(m_player.get());
    for (auto &ball : m_balls) {
        m_gameObjects.push_back(ball.get());
    }
    for (auto &obj : m_gameObjects) {
        if (!obj->isAlive()) continue;
        obj->fixedUpdate(delta);
    }
}

void Game::render(float alpha) {
    if (currentState == State::Menu) return;

    if (!m_player->isHidden()) {
        m_renderer->submit({RenderLayer::Player, m_player->getShader(), m_player->getTexture(),
                            core::lerp(m_player->getPreviousPosition(), m_player->getPosition(), alpha), m_player->getSize(), 0.0f,
                            m_player->getColor()});
    }

    for (const auto &brick : currentLevel.getBricks()) {
        if (brick->isHidden()) continue;
        m_renderer->submit(
            {RenderLayer::Brick, brick->getShader(), brick->getTexture(), brick->getPosition(), brick->getSize(), 0.0f, brick->getColor()});
    }

    for (auto &ball : m_balls) {
        if (ball->isHidden()) continue;
        m_renderer->submit({RenderLayer::Ball, ball->getShader(), ball->getTexture(),
                            core::lerp(ball->getPreviousPosition(), ball->getPosition(), alpha), ball->getSize(), 0.0f, ball->getColor()});
    }

    for (auto &powerUp : currentLevel.getPowerUps()) {
        if (powerUp->isHidden()) continue;
        m_renderer->submit({RenderLayer::PowerUp, powerUp->getShader(), powerUp->getTexture(),
                            core::lerp(powerUp->getPreviousPosition(), powerUp->getPosition(), alpha), powerUp->getSize(), 0.0f,
                            powerUp->getColor()});
    }

    m_renderer->submit(
        {RenderLayer::Background, m_background.getShader(), m_background.getTexture(), m_background.getPosition(), m_background.getSize()});

    m_renderer->flush();
}

void Game::renderText(float alpha) {
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
    modifiers.reset();
    clearBalls();
    keepStuckBalls();
    currentLevel.clearPowerUps();
    resetPlayer();
    resetHeroBall();
    repositionStuckBallsOnPlayer();
    currentLevel.cleanup();
    m_objectManager->cleanup();
    ++m_currentLevelNumber;
    currentLevel = m_levelGenerator->generate(5, 5);
    currentLevel.load();
    currentLevel.setPosition(currentLevel.getPosition() + glm::vec2(0.0f, -5.0f));
    logging::Info("Next level is loaded");
}

void Game::resetLevel(GameLevel &level) {
    modifiers.reset();
    clearStuckBalls();
    clearBalls();
    currentLevel.clearPowerUps();
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
    ball->setVelocity(BALL_INITIAL_VELOCITY);
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

void Game::doCollisions() {
    for (auto &poweup : currentLevel.getPowerUps()) {
        if (!poweup->isAlive()) continue;

        poweup->checkCollision(*m_player);
    }
    for (auto &ball : m_balls) {
        if (!ball->isAlive()) continue;

        Collision playerCollision = ball->checkCollision(*m_player);
        if (get<CollisionDataType::IsCollided>(playerCollision)) {
            m_context->getEventDispatcher().emit(
                BallHit{ball, get<CollisionDataType::CollisionPoint>(playerCollision), CollisionType::Player});
            if (modifiers.stickyPlayer) {
                stickBallToPlayer(ball);
            }
        }

        for (auto &brick : currentLevel.getBricks()) {
            if (!brick->isAlive() || modifiers.passThrough) continue;

            Collision brickCollision = ball->checkCollision(*brick);
            if (get<CollisionDataType::IsCollided>(brickCollision)) {
                CollisionDirection dir = get<CollisionDataType::CollisionDirection>(brickCollision);
                glm::vec2 diffVector = get<CollisionDataType::DifferenceVector>(brickCollision);
                glm::vec2 collisionPoint = get<CollisionDataType::CollisionPoint>(brickCollision);
                m_collisionPointHistory.push_back(collisionPoint);
                m_context->getEventDispatcher().emit(BallHit{ball, collisionPoint, CollisionType::Brick});
                _calcBallNewPositionAndVelocity(*ball, dir, diffVector);

                if (brick->getPowerUpType() != PowerUpType::None) {
                    currentLevel.spawnPowerUp(brick->getPowerUpType(), brick->getPosition());
                }

                if (currentLevel.isFinished()) {
                    m_context->getEventDispatcher().emit(GameFinished{true});
                }
            }
        }
    }
}

void Game::setGodMode(bool enabled) noexcept {
    modifiers.godPlayer = enabled;
}

bool Game::isGodModeEnabled() const noexcept {
    return modifiers.godPlayer;
}

void Game::_onBallFliedOff(const BallFliedOff &e) {
    if (e.ball == m_heroBall) {
        if (!modifiers.godPlayer && currentState == State::Active && ++m_currentAttempt >= m_attempts) {
            m_context->getEventDispatcher().emit(GameFinished{false});
        } else {
            stickBallToPlayer(m_heroBall);
        }
    } else {
        e.ball->destroy();
    }
}

void Game::_onPowerUpActivated(const PowerUpActivated &e) {
    for (auto &currentPowerUp : currentLevel.getPowerUps()) {
        if (currentPowerUp->isActivated() && currentPowerUp != e.powerUp &&
            currentPowerUp->getPowerUpType() == e.powerUp->getPowerUpType()) {
            currentPowerUp->reset();
            e.powerUp->destroy();
            logging::Info("PowerUp: {} is reseted", to_string(e.powerUp->getPowerUpType()));
            return;
        }
    }

    switch (e.powerUp->getPowerUpType()) {
        case PowerUpType::SpeedBall:
            m_heroBall->setSpeed(BALL_DEFAULT_SPEED * fastrand::randomFloatInRange(1.5f, 2.0f));
            break;
        case PowerUpType::WidePlayer:
            m_player->setSize(glm::vec2(PLAYER_DEFAULT_SIZE.x * 1.5f, PLAYER_DEFAULT_SIZE.y));
            repositionStuckBallsOnPlayer();
            modifiers.widePlayer = true;
            break;
        case PowerUpType::StickyPlayer:
            modifiers.stickyPlayer = true;
            break;
        case PowerUpType::PassTrough:
            modifiers.passThrough = true;
            break;
        case PowerUpType::None:
        default:
            break;
    }
    m_context->getAudioManager().playOnce(POWERUP_PICKUP_SOUND);
    logging::Info("PowerUp: {} is activated", to_string(e.powerUp->getPowerUpType()));
}

void Game::_onPowerUpFinished(const PowerUpFinished &e) {
    switch (e.powerUp->getPowerUpType()) {
        case PowerUpType::SpeedBall:
            m_heroBall->setSpeed(BALL_DEFAULT_SPEED);
            break;
        case PowerUpType::WidePlayer:
            m_player->setSize(PLAYER_DEFAULT_SIZE);
            repositionStuckBallsOnPlayer();
            modifiers.widePlayer = false;
            break;
        case PowerUpType::StickyPlayer:
            modifiers.stickyPlayer = false;
            break;
        case PowerUpType::PassTrough:
            modifiers.passThrough = false;
            break;
        case PowerUpType::None:
        default:
            break;
    }
    logging::Info("PowerUp: {} is finished", to_string(e.powerUp->getPowerUpType()));
}

void Game::_onBallHit(const BallHit &e) {
    float panValue = (e.position.x / core::getWorldAABB().z);
    m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_PAN, panValue);
    if (e.collisionType == CollisionType::Player && modifiers.stickyPlayer) {
        m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_SPEED, 0.0f);
        return;
    } else {
        m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_SPEED, e.ball->getSpeed());
    }
    switch (e.collisionType) {
        case CollisionType::Brick:
            m_context->getAudioManager().playOnce(BALL_HIT_BRICK_SOUND, e.position, e.ball->getVelocity() * e.ball->getSpeed());
            break;
        case CollisionType::Obstacle:
            m_context->getAudioManager().playOnce(BALL_HIT_OBSTACLE_SOUND, e.position, e.ball->getVelocity() * e.ball->getSpeed());
            break;
        case CollisionType::Player:
            if (modifiers.widePlayer)
                m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_PITCH, 0.0f);
            else if (modifiers.narrowPlayer)
                m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_PITCH, 1.0f);
            else
                m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_PITCH, 0.5f);
            m_context->getAudioManager().playOnce(BALL_HIT_PLAYER_SOUND, e.position, e.ball->getVelocity() * e.ball->getSpeed());
            break;
        case CollisionType::None:
        default:
            break;
    }
}

void Game::_onPowerUpSpawned(const PowerUpSpawned &e) {}

void Game::_onBallUnstuck(const BallUnstuck &e) {
    m_context->getAudioManager().changeGlobalParameter(AUDIO_PARAMETER_SPEED, e.ball->getSpeed());
    m_context->getAudioManager().playOnce(BALL_WHOOSH_SOUND, e.ball->getPosition());
}

void Game::_onBallStuck(const BallStuck &e) {
    m_context->getAudioManager().playOnce(BALL_STUCK_SOUND);
}

void Game::_onGameFinished(const GameFinished &e) {
    currentState = e.playerWon ? State::Win : State::Defeat;
    if (e.playerWon) {
        logging::Info("Player won!");
    } else {
        logging::Info("Player defeated!");
    }
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
        currentLevel.setPosition(currentLevel.getPosition() + glm::vec2(0.0, dt));
        co_await std::suspend_always{};
    }
    while (std::sin(timer::time()) < 0.0f) {
        currentLevel.setPosition(currentLevel.getPosition() + glm::vec2(0.0, -dt));
        co_await std::suspend_always{};
    }
}
