#pragma once

#include "collision_type.hpp"
#include "game_level.hpp"
#include "sprite_renderer.hpp"

#include <atomic>
#include <glm/glm.hpp>
#include <memory>
#include <thread>
#include <vector>

// fwd
class GameObject;
class Shader;
class TextRenderer;
class Ball;
class Player;
class ParticleEmitter;
//

#define GAME_NAME "BRAKEOUT 2D"
#define INITIAL_BALL_VELOCITY (glm::vec2(0.5f, 1.0f))
#define PLAYER_DEFAULT_SIZE (glm::vec2(256.0f, 32.0f))
#define PLAYER_START_POSITION (glm::vec2(Window::getWidth() / 2.0f - PLAYER_DEFAULT_SIZE.x / 2.0f, 25.0f))

enum GameState {
    GAME_NONE,
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
  private:
    std::shared_ptr<Shader> m_spriteShader;
    std::shared_ptr<Shader> m_textShader;
    std::shared_ptr<Shader> m_particleShader;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<ParticleEmitter> m_particleEmitterBall;
    std::vector<std::unique_ptr<Ball>> m_balls;
    std::vector<GameLevel> m_levels;
    unsigned int m_currentLevelNumber = 0;
    unsigned int m_attempts = 0;
    unsigned int m_currentAttempt = 0;
    GameLevel m_currentLevel;
    std::atomic<bool> m_running = false;
    std::thread m_consoleInputThread;

    glm::vec2 _lerpPos(GameObject &gameObject, float alpha);
    void _calcBallNewPositionAndVelocity(Ball &ball, CollisionDirection dir, glm::vec2 diffVector);

  public:
    GameState CurrentState = GAME_NONE;
    bool Keys[1024] = {false};
    unsigned int Width = 0;
    unsigned int Height = 0;

    Game(unsigned int width, unsigned int height, unsigned int attempts);
    ~Game();

    void init();
    void processInput(float dt);
    void update(float dt);
    void fixedUpdate(float dt);
    void render(float alpha);
    void doCollisions();
    void nextLevel();
    GameLevel getLevel(unsigned int number);
    void restartCurrentLevel();
    void resetBalls();
    void resetBallPosition(Ball &ball);
    void resetPlayer();
    void cleanDestroyedBalls();

    void spawnBall();
};
