#pragma once

#include "collision_detection.hpp"
#include "game_level.hpp"
#include "sprite_rendered.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#include <memory>
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
#define PLAYER_START_POSITION (glm::vec2(Window::getWidth() / 2 - PLAYER_DEFAULT_SIZE.x / 2, 25.0f))

enum GameState {
	GAME_NONE,
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game {
private:
	std::shared_ptr<Shader> m_spriteShader = nullptr;
	std::shared_ptr<Shader> m_textShader = nullptr;
	std::shared_ptr<Shader> m_particleShader = nullptr;
	std::unique_ptr<SpriteRenderer> m_spriteRenderer = nullptr;
	std::unique_ptr<TextRenderer> m_textRenderer = nullptr;
	std::unique_ptr<Player> m_player = nullptr;
	std::unique_ptr<Ball> m_ball = nullptr;
	std::unique_ptr<ParticleEmitter> m_particleEmitterBall = nullptr;
	std::vector<GameLevel> m_levels = { };
	unsigned int m_currentLevelNumber = 0;
	unsigned int m_attempts = 0;
	unsigned int m_currentAttempt = 0;
	GameLevel m_currentLevel;

	glm::vec2 _lerpPos(GameObject &gameObject, float alpha);
	void _calcBallNewPositionAndVelocity(CollisionDirection dir, glm::vec2 diffVector);

public:
	GameState GameState = GameState::GAME_NONE;
	bool Keys[1024] = { false };
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
	void resetBall();
	void resetPlayer();
};