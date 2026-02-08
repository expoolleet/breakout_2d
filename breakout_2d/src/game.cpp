#include "pch.hpp"

#include "ball.hpp"
#include "collision_type.hpp"
#include "event_dispatcher.hpp"
#include "event_type.hpp"
#include "game.hpp"
#include "game_level.hpp"
#include "game_object.hpp"
#include "logging.hpp"
#include "particle_emitter.hpp"
#include "path_manager.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "sprite_rendered.hpp"
#include "text_renderer.hpp"
#include "texture_manager.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#include <format>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <tuple>


glm::vec2 Game::_lerpPos(GameObject &gameObject, float alpha) {
	return glm::mix(gameObject.getPreviousPosition(), gameObject.getPosition(), alpha);
}

void Game::_calcBallNewPositionAndVelocity(CollisionDirection dir, glm::vec2 diffVector) {
	glm::vec2 ballVelocity = m_ball->getVelocity();
	glm::vec2 ballPosition = m_ball->getPosition();
	if (dir == COLLISION_DIRECTION_LEFT || dir == COLLISION_DIRECTION_RIGHT) {
		ballVelocity.x = -ballVelocity.x;
		float shift = m_ball->getRadius() - std::abs(diffVector.x);
		if (dir == COLLISION_DIRECTION_LEFT) {
			ballPosition.x += shift;
		}
		else {
			ballPosition.x -= shift;
		}
	}
	else {
		ballVelocity.y = -ballVelocity.y;
		float shift = m_ball->getRadius() - std::abs(diffVector.y);
		if (dir == COLLISION_DIRECTION_UP) {
			ballPosition.y -= shift;
		}
		else {
			ballPosition.y += shift;
		}
	}
	m_ball->setVelocity(ballVelocity);
	m_ball->setPosition(ballPosition);
}

Game::Game(unsigned int width, unsigned int height, unsigned int attempts) : Width(width), Height(height), m_attempts(attempts) {
	GameState = GameState::GAME_ACTIVE;
}

Game::~Game() {
	// add later cleaning
	m_spriteRenderer.release();
}

void Game::init() {
	glm::mat4 projectionMat = glm::ortho(0.0f, float(Window::getWidth()), 0.0f, float(Window::getHeight()));

	std::string shadersPath = PathManager::getResourcePath("shaders");
	m_spriteShader = std::make_shared<Shader>(shadersPath + "/vs/sprite.vs", shadersPath + "/fs/sprite.fs");
	m_spriteShader->setMat4("projection", projectionMat);
	m_spriteShader->setInt("sprite", 0);
	m_spriteRenderer = std::make_unique<SpriteRenderer>();

	m_textShader = std::make_shared<Shader>(shadersPath + "/vs/text.vs", shadersPath + "/fs/textMSDF.fs");
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
	m_currentLevelNumber = 2;
	m_currentLevel = m_levels[m_currentLevelNumber - 1];
	m_currentLevel.load();

	m_player = std::make_unique<Player>(TextureManager::getTexture("player"), PLAYER_START_POSITION, PLAYER_DEFAULT_SIZE);
	m_player->setSpeed(500.0f);

	m_ball = std::make_unique<Ball>(TextureManager::getTexture("ball"), glm::vec2(0.0f), glm::vec2(32.0f), *m_player);
	m_ball->setVelocity(INITIAL_BALL_VELOCITY);
	m_ball->setRadius(m_ball->getSize().x / 2);
	m_ball->setSpeed(800.0f);
	m_ball->setDamage(1);

	m_particleShader = std::make_shared<Shader>(shadersPath + "/vs/particle.vs", shadersPath + "/fs/particle.fs");
	m_particleShader->setMat4("projection", projectionMat);
	m_particleShader->setInt("sprite", 0);
	m_particleEmitterBall = std::make_unique<ParticleEmitter>(TextureManager::getTexture("ball"), 300);
	m_particleEmitterBall->setParticleDelay(0.9f);
	m_particleEmitterBall->setParticleLifeTime(1.0f);
	m_particleEmitterBall->setParticleAttenuationSpeed(2.5f);
	m_particleEmitterBall->setParticleScale(15.0f);
	m_particleEmitterBall->init();

	EventDispatcher::Get().subscribe<BallFliedOff>([this](const BallFliedOff& e) {
		if (++m_currentAttempt >= m_attempts) {
			this->restartCurrentLevel();
		}
		else {
			resetBall();
		}
	});
}

void Game::processInput(float dt) {
	if (GameState == GameState::GAME_WIN) {
		if (Keys[GLFW_KEY_ENTER]) {
			nextLevel();
		}
	}
	if (GameState != GameState::GAME_ACTIVE)
		return;

	glm::vec2 velocity = m_player->getVelocity();
	if (Keys[GLFW_KEY_A]) {
		velocity.x = -1.0f;
	}
	else if (Keys[GLFW_KEY_D]) {
		velocity.x = 1.0f;
	}
	else {
		velocity.x = 0.0f;
	}
	m_player->setVelocity(velocity);

	if (Keys[GLFW_KEY_SPACE]) {
		m_ball->setStuck(false);
	}
}

void Game::update(float dt) {

}

void Game::fixedUpdate(float dt) {
	if (GameState != GameState::GAME_ACTIVE)
		return;
	m_player->fixedUpdate(dt);
	m_ball->fixedUpdate(dt);
	m_particleEmitterBall->update(dt, *m_ball, 2, glm::vec2(m_ball->getRadius() / 2));
	doCollisions();
}

void Game::render(float alpha) {
	if (GameState == GameState::GAME_MENU)
		return;


	// objects
	m_spriteShader->use();
	m_spriteRenderer->drawSprite(*m_spriteShader, TextureManager::getTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(float(Window::getWidth()), float(Window::getHeight())));
	for (const auto &brick : m_currentLevel.getBricks()) {
		if (!brick.IsHidden) {
			m_spriteRenderer->drawSprite(*m_spriteShader, *brick.Texture, brick.getPosition(), brick.getSize(), 0.0f, brick.Color);
		}
	}

	m_spriteRenderer->drawSprite(*m_spriteShader, *m_player->Texture, _lerpPos(*m_player, alpha), m_player->getSize());

	m_particleShader->use();
	m_particleEmitterBall->emit(*m_particleShader);
	m_spriteShader->use();
	m_spriteRenderer->drawSprite(*m_spriteShader, *m_ball->Texture, _lerpPos(*m_ball, alpha), m_ball->getSize());
	// text
	m_textShader->use();
	if (GameState == GameState::GAME_WIN) {
		m_textRenderer->renderMSDF(*m_textShader, "YOU WON! :)", Window::getWidth() / 2 - 350, Window::getHeight() / 2, 3.0f, glm::vec3(0.0f, 0.9f, 0.6f), false, {glm::vec3(0.0f), 5.0f});
	}
	else {
		m_textRenderer->renderMSDF(*m_textShader, GAME_NAME, Window::getWidth() / 2 - 150, Window::getHeight() / 2, 1.0f, glm::vec3(1.0f), true, { glm::vec3(0.0f), 2.0f });
	}
	m_textRenderer->renderMSDF(*m_textShader, std::to_string(m_attempts - m_currentAttempt), 15, Window::getHeight() - 50);

	m_textRenderer->renderMSDF(*m_textShader, std::format("Ball velocity.x: {}; velocity.y: {}", m_ball->getVelocity().x, m_ball->getVelocity().y), 50, 5, 0.5f, glm::vec3(1.0f), false, { glm::vec3(0.0f), 1.0f });
}

void Game::doCollisions() {
	m_ball->checkCollision(*m_player);
	for (auto &brick : m_currentLevel.getBricks()) {
		if (brick.getCurrentHardnessPoints() == 0)
			continue;
		Collision collision = brick.checkCollision(*m_ball);
		if (std::get<0>(collision)) {
			CollisionDirection dir = std::get<1>(collision);
			glm::vec2 diffVector = std::get<2>(collision);
			_calcBallNewPositionAndVelocity(dir, diffVector);
			if (m_currentLevel.isFinished()) {
				GameState = GameState::GAME_WIN;
			}
		}
	}
}

void Game::nextLevel() {
	resetPlayer();
	resetBall();
	m_currentLevel = getLevel(++m_currentLevelNumber);
	m_currentLevel.load();
	GameState = GameState::GAME_ACTIVE;
}

GameLevel Game::getLevel(unsigned int number) {
	if (number > m_levels.size()) {
		Logging::Warn(std::format("Could not return level with number {} because level count is {}. Returned first level.", number, m_levels.size()));
		return m_levels[0];
	}
	return m_levels[number - 1];
}

void Game::restartCurrentLevel() {
	resetPlayer();
	resetBall();
	m_currentLevel.restart();
	m_currentAttempt = 0;
}

void Game::resetBall() {
	m_ball->setStuck(true);
	glm::vec2 ballPosition = m_player->getPosition() + glm::vec2(m_player->getSize().x / 2, m_ball->getSize().y);
	m_ball->resetPosition(ballPosition);
}

void Game::resetPlayer() {
	m_player->resetPosition(PLAYER_START_POSITION);
}
