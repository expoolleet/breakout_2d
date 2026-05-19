#pragma once

#include <atomic>
#include <glm/glm.hpp>
#include <memory>
#include <thread>
#include <vector>

#include "ball.hpp"
#include "collision_type.hpp"
#include "engine_context.hpp"
#include "event_type.hpp"
#include "game_core.hpp"
#include "game_level.hpp"
#include "game_object.hpp"
#include "particle_emitter.hpp"
#include "player.hpp"
#include "powerup.hpp"
#include "powerup_type.hpp"
#include "shader.hpp"
#include "sprite_renderer.hpp"
#include "task.hpp"
#include "text_renderer.hpp"

constexpr std::string GAME_NAME = "BRAKEOUT 2D";
constexpr glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(0.0f, 1.0f);
constexpr glm::vec2 PLAYER_DEFAULT_SIZE = glm::vec2(4.0f, 0.65f);
constexpr glm::vec2 PLAYER_START_POSITION = glm::vec2(-PLAYER_DEFAULT_SIZE.x / 2.0f, -(core::getWorldHeight() / 2.0f) + 1.0f);
constexpr int MAX_BALL_DAMAGE = 3;
constexpr int MIN_BALL_DAMAGE = 1;
constexpr int MAX_KEY_CODE = 512;

enum class GameState {
    None,
    Active,
    Menu,
    Win
};

class Game {
   private:
    bool m_keys[MAX_KEY_CODE] = {false};
    std::vector<GameLevel> m_levels;
    std::vector<PowerUp> m_powerups;
    std::vector<std::unique_ptr<Ball>> m_balls;
    std::vector<std::unique_ptr<Ball>> m_queueBalls;
    std::vector<Ball *> m_stuckBalls;
    std::shared_ptr<Shader> m_spriteShader;
    std::shared_ptr<Shader> m_textShader;
    std::shared_ptr<Shader> m_particleShader;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<ParticleEmitter> m_ballParticles;
    std::unique_ptr<ParticleEmitter> m_collisionHitParticles;
    std::unique_ptr<Player> m_player;
    GameLevel m_currentLevel;
    Ball *m_heroBall = nullptr;  // observer pointer
    int m_currentLevelNumber = 0;
    int m_attempts = 0;
    int m_currentAttempt = 0;
    size_t m_maxCountBallsStuckToPlayer = 0;
    int m_particlesPerFrame = 2;
    std::atomic<bool> m_running = false;
    std::thread m_consoleInputThread;
    float m_nameSize = 0.07f;
    Context &m_context;

    std::vector<glm::vec2> m_collisionPointHistory;

    glm::vec2 _lerpPos(GameObject &gameObject, float alpha);
    void _calcBallNewPositionAndVelocity(Ball &ball, CollisionDirection dir, glm::vec2 diffVector);

   public:
    GameState currentState = GameState::None;

    Game(unsigned int attempts);
    ~Game();

    void init();
    void processInput(float dt);
    void pressKey(int key);
    void unpressKey(int key);
    bool isKeyPressed(int key);
    void update(float dt);
    void fixedUpdate(float dt);
    void render(float alpha);
    void renderText(float alpha);
    void doCollisions();
    void nextLevel();
    void restartCurrentLevel();
    void resetHeroBall();
    void resetBallPosition(Ball &ball);
    void resetPlayer();
    void setProjectionMatrix();
    void cleanDestroyedBalls();
    void destroyBallsExceptHeroBall();
    void spawnBall(glm::vec2 position);
    void spawnPowerUp(PowerUpType type, glm::vec2 position);
    void updatePowerUps(float dt);
    void cleanupPowerUps();
    void repositionStuckBallsOnPlayer();
    void stickBallToPlayer(Ball &ball);
    void unstickBallFromPlayer(Ball &ball);
    void clearStuckBallsExceptHeroBall();

    GameLevel getLevel(unsigned int number);
    std::vector<std::unique_ptr<Ball>> &getBalls();

    // handlers
    void onBallFliedOff(const BallFliedOff &e);
    void onPowerUpActivated(const PowerUpActivated &e);
    void onPowerUpFinished(const PowerUpFinished &e);
    void onBallHit(const BallHit &e);
    void onBallUnstuck(const BallUnstuck &e);
    void onBallStuck(const BallStuck &e);

    // coroutines
    Task animateName(float dt);
};
