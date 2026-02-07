#pragma once

#include "game_level.hpp"
#include "sprite_rendered.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <tuple>
#include <vector>

// fwd
class GameObject;
class Shader;
class TextRenderer;
class Ball;
class Player;
//

#define GAME_NAME "BRAKEOUT 2D"
#define INITIAL_BALL_VELOCITY (glm::vec2(0.25f, 1.0f))
#define PLAYER_DEFAULT_SIZE (glm::vec2(256.0f, 32.0f))
#define PLAYER_START_POSITION (glm::vec2(Window::getWidth() / 2 - PLAYER_DEFAULT_SIZE.x / 2, 25.0f))

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

enum GameState {
	GAME_NONE,
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
private:
	std::shared_ptr<Shader> m_spriteShader = nullptr;
	std::shared_ptr<Shader> m_textShader = nullptr;
	std::unique_ptr<SpriteRenderer> m_spriteRenderer = nullptr;
	std::unique_ptr<TextRenderer> m_textRenderer = nullptr;
	std::unique_ptr<Player> m_player = nullptr;
	std::unique_ptr<Ball> m_ball = nullptr;
	std::vector<GameLevel> m_levels = { };
	unsigned int m_currentLevelNumber = 0;
	GameLevel m_currentLevel;

	glm::vec2 _lerpPos(GameObject &gameObject, float alpha);
	Direction _getDirection(glm::vec2 target);
	void _calcBallNewPositionAndVelocity(Direction dir, glm::vec2 diffVector);

public:
	GameState GameState = GameState::GAME_NONE;
	bool Keys[1024] = { false };
	unsigned int Width = 0;
	unsigned int Height = 0;

	Game(unsigned int width, unsigned int height);
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
	Collision checkCollision(GameObject &a, GameObject& b);
	Collision checkCollision(Ball &ball, GameObject& gameObject);
};