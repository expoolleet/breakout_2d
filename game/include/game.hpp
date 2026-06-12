#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "background.hpp"
#include "ball.hpp"
#include "collision_type.hpp"
#include "engine_context.hpp"
#include "event_type.hpp"
#include "game_level.hpp"
#include "game_level_generator.hpp"
#include "game_modifiers.hpp"
#include "game_renderer.hpp"
#include "game_scene.hpp"
#include "input.hpp"
#include "object_manager.hpp"
#include "particle_emitter.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "task.hpp"
#include "text_renderer.hpp"

inline constexpr std::string_view GAME_NAME = "Brakeout 2D";

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
    GameLevel currentLevel;
    GameModifiers modifiers;

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
    void repositionStuckBallsOnPlayer();
    void stickBallToPlayer(BallPtr ball);
    void unstickBallFromPlayer(BallPtr ball);

    // testing
    void setGodMode(bool enabled) noexcept;
    bool isGodModeEnabled() const noexcept;

    GameLevel getLevel(int lvlNumber);

   private:
    std::vector<GameLevel> m_levels;
    std::vector<BallPtr> m_stuckBalls;
    std::vector<BallPtr> m_balls;
    std::vector<BallPtr> m_queueBalls;
    std::vector<glm::vec2> m_collisionPointHistory;

    Scene m_mainScene;

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

    int m_attempts;
    int m_currentAttempt = 0;
    int m_currentLevelNumber = 0;
    int m_ballTrailParticlesPerFrame = 2;
    int m_collisionParticlesPerFrame = 30;

    size_t m_maxCountBallsStuckToPlayer = 0;

    float m_nameSize = 1.0f;

    void _calcBallNewPositionAndVelocity(Ball &ball, CollisionDirection dir, glm::vec2 diffVector);

    // handlers
    void _onBallFliedOff(const BallFliedOff &e);
    void _onPowerUpActivated(const PowerUpActivated &e);
    void _onPowerUpFinished(const PowerUpFinished &e);
    void _onBallHit(const BallHit &e);
    void _onBallUnstuck(const BallUnstuck &e);
    void _onBallStuck(const BallStuck &e);
    void _onGameFinished(const GameFinished &e);
    void _onPowerUpSpawned(const PowerUpSpawned &e);

    // coroutines
    Task _animateName(float dt);
    Task _moveScene(float dt);
};
