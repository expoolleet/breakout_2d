#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "background.hpp"
#include "ball.hpp"
#include "collision_type.hpp"
#include "engine_context.hpp"
#include "event_type.hpp"
#include "game_core.hpp"
#include "game_level.hpp"
#include "game_level_generator.hpp"
#include "game_renderer.hpp"
#include "input.hpp"
#include "object_manager.hpp"
#include "particle_emitter.hpp"
#include "player.hpp"
#include "powerup.hpp"
#include "powerup_type.hpp"
#include "shader.hpp"
#include "task.hpp"
#include "text_renderer.hpp"

constexpr std::string_view GAME_NAME = "BRAKEOUT 2D";
constexpr glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(0.0f, 1.0f);
constexpr glm::vec2 PLAYER_DEFAULT_SIZE = glm::vec2(4.0f, 0.65f);
constexpr glm::vec2 PLAYER_START_POSITION = glm::vec2(-PLAYER_DEFAULT_SIZE.x / 2.0f, -(core::getWorldHeight() / 2.0f) + 1.0f);
constexpr int MAX_BALL_DAMAGE = 3;
constexpr int MIN_BALL_DAMAGE = 1;

struct GameCreateInfo {
    ContextPtr contextPtr;

    ShaderPtr spriteShaderPtr;
    ShaderPtr textShaderPtr;
    ShaderPtr particleShaderPtr;

    GameRendererPtr gameRendererPtr;
    TextRendererPtr textRendererPtr;

    ParticleEmitterPtr ballParticleEmitterPtr;
    ParticleEmitterPtr collisionHitParticleEmitterPtr;

    ObjectManagerPtr objectManagerPtr;

    int gameAttempts;
};

class Game {
   public:
    enum class State {
        None,
        Menu,
        Active,
        Win,
        Defeat,
    };

    Keys keys;
    State currentState;

    Game(GameCreateInfo createInfo);
    ~Game() = default;

    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    Game(Game &&) = delete;
    Game &operator=(Game &&) = delete;

    void init();
    void processInput(float dt);
    void update(float dt);
    void fixedUpdate(float dt);
    void render(float alpha);
    void renderText(float alpha);
    void doCollisions();
    void nextLevel();
    void resetLevel(GameLevel &level);
    void resetHeroBall();
    void resetBallPosition(BallPtr ball);
    void resetPlayer();
    void setProjectionMatrix();
    void clearBalls();
    void clearStuckBalls();
    void keepStuckBalls();
    void appendQueueBalls();
    void updateBalls(float dt);
    void updateParticles(float dt);
    void spawnBall(glm::vec2 position);
    void spawnPowerUp(PowerUpType type, glm::vec2 position);
    void updatePowerUps(float dt);
    void eraseFinishedPowerUps();
    void clearPowerUps();
    void repositionStuckBallsOnPlayer();
    void stickBallToPlayer(BallPtr ball);
    void unstickBallFromPlayer(BallPtr ball);

    // testing
    void setGodMode(bool enabled) noexcept;
    bool isGodModeEnabled() const noexcept;

    GameLevel getLevel(int lvlNumber);

   private:
    std::vector<GameLevel> m_levels;
    std::vector<PowerUpPtr> m_powerups;
    std::vector<BallPtr> m_stuckBalls;
    std::vector<BallPtr> m_balls;
    std::vector<BallPtr> m_queueBalls;
    std::vector<glm::vec2> m_collisionPointHistory;

    ShaderPtr m_spriteShader;
    ShaderPtr m_textShader;
    ShaderPtr m_particleShader;

    BallPtr m_heroBall;

    PlayerPtr m_player;

    ContextPtr m_context;

    ObjectManagerPtr m_objectManager;

    std::unique_ptr<PowerUpFactory> m_powerUpFactory;
    std::unique_ptr<GameLevelGenerator> m_levelGenerator;

    GameRendererPtr m_renderer;
    TextRendererPtr m_textRenderer;
    ParticleEmitterPtr m_ballParticleEmitter;
    ParticleEmitterPtr m_collisionHitParticleEmitter;

    Background m_background;

    GameLevel m_currentLevel;

    int m_attempts;
    int m_currentAttempt = 0;
    int m_currentLevelNumber = 0;
    int m_ballTrailParticlesPerFrame = 2;
    int m_collisionParticlesPerFrame = 30;

    size_t m_maxCountBallsStuckToPlayer = 0;

    float m_nameSize = 2.0f;

    bool m_godMode = false;

    void _calcBallNewPositionAndVelocity(Ball &ball, CollisionDirection dir, glm::vec2 diffVector);

    // handlers
    void _onBallFliedOff(const BallFliedOff &e);
    void _onPowerUpActivated(const PowerUpActivated &e);
    void _onPowerUpFinished(const PowerUpFinished &e);
    void _onBallHit(const BallHit &e);
    void _onBallUnstuck(const BallUnstuck &e);
    void _onBallStuck(const BallStuck &e);
    void _onGameFinished(const GameFinished &e);

    // coroutines
    Task _animateName(float dt);
};
