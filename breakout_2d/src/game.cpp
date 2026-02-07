#include "pch.hpp"

#include "ball.hpp"
#include "game.hpp"
#include "game_level.hpp"
#include "game_object.hpp"
#include "glm/glm.hpp"
#include "particle_emitter.hpp"
#include "logging.hpp"
#include "path_manager.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "sprite_rendered.hpp"
#include "text_renderer.hpp"
#include "texture_manager.hpp"
#include "window.hpp"

#include <format>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <tuple>


glm::vec2 Game::_lerpPos(GameObject &gameObject, float alpha) {
	return glm::mix(gameObject.getPreviousPosition(), gameObject.getPosition(), alpha);
}

Direction Game::_getDirection(glm::vec2 target) {
	static glm::vec2 directions[4] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f),
	};

	float maxContribution = 0.0f;
	unsigned int bestMatch = -1;
	for (unsigned int i = 0; i < 4; ++i) {
		float cosAngle = glm::dot(directions[i], glm::normalize(target));
		if (cosAngle > maxContribution) {
			maxContribution = cosAngle;
			bestMatch = i;
		}
	}
	return (Direction)bestMatch;
}

void Game::_calcBallNewPositionAndVelocity(Direction dir, glm::vec2 diffVector) {
	glm::vec2 ballVelocity = m_ball->getVelocity();
	glm::vec2 ballPosition = m_ball->getPosition();
	if (dir == Direction::LEFT || dir == Direction::RIGHT) {
		ballVelocity.x = -ballVelocity.x;
		float shift = m_ball->getRadius() - std::abs(diffVector.x);
		if (dir == Direction::LEFT) {
			ballPosition.x += shift;
		}
		else {
			ballPosition.x -= shift;
		}
	}
	else {
		ballVelocity.y = -ballVelocity.y;
		float shift = m_ball->getRadius() - std::abs(diffVector.y);
		if (dir == Direction::UP) {
			ballPosition.y -= shift;
		}
		else {
			ballPosition.y += shift;
		}
	}
	m_ball->setVelocity(ballVelocity);
	m_ball->setPosition(ballPosition);
}

Game::Game(unsigned int width, unsigned int height) : Width(width), Height(height) {
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
	m_levels.push_back(GameLevel(levelsPath + "/1.lvl", Window::getWidth(), Window::getHeight() / 2));
	m_levels.push_back(GameLevel(levelsPath + "/2.lvl", Window::getWidth(), Window::getHeight() / 2));
	m_levels.push_back(GameLevel(levelsPath + "/3.lvl", Window::getWidth(), Window::getHeight() / 2));
	m_levels.push_back(GameLevel(levelsPath + "/4.lvl", Window::getWidth(), Window::getHeight()));
	m_currentLevelNumber = 2;
	m_currentLevel = m_levels[m_currentLevelNumber - 1];
	m_currentLevel.load();

	m_player = std::make_unique<Player>(TextureManager::getTexture("player"), PLAYER_START_POSITION, PLAYER_DEFAULT_SIZE);
	m_player->setSpeed(500.0f);

	m_ball = std::make_unique<Ball>(TextureManager::getTexture("ball"), glm::vec2(0.0f), glm::vec2(32.0f));
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
	float offset = 5.0f;
	glm::vec2 playerPosition = m_player->getPosition();
	playerPosition += m_player->getVelocity() * m_player->getSpeed() * dt;
	int leftSide = 0.0f + offset;
	if (playerPosition.x <= leftSide) {
		playerPosition.x = leftSide;
 	} 
	float rightSide = Window::getWidth() - m_player->getSize().x - offset;
	if (playerPosition.x >= rightSide) {
		playerPosition.x = rightSide;
	}
	m_player->setPosition(playerPosition);

	if (m_ball->isStuck()) {	
		glm::vec2 ballPosition = m_player->getPosition() + glm::vec2(m_player->getSize().x / 2, m_ball->getSize().y);
		m_ball->setPosition(ballPosition);
	}
	else {
		m_ball->move(dt, static_cast<float>(Window::getWidth()), static_cast<float>(Window::getHeight()));
	}

	if (m_ball->getPosition().y <= 0.0f || m_ball->getPosition().y + m_ball->getSize().y >= Window::getHeight()) {
		restartCurrentLevel();
	}

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

	m_textRenderer->renderMSDF(*m_textShader, std::format("Ball velocity.x: {}; velocity.y: {}", m_ball->getVelocity().x, m_ball->getVelocity().y), 80, 10, 0.6f, glm::vec3(1.0f), false, { glm::vec3(0.0f), 1.0f });
}

void Game::doCollisions() {
	Collision playerCollision = checkCollision(*m_ball, *m_player);
	if (!m_ball->isStuck() && std::get<0>(playerCollision)) {
		float centerBoard = m_player->getPosition().x + m_player->getSize().x / 2;
		float distance = (m_ball->getPosition().x + m_ball->getRadius()) - centerBoard;
		float percentage = distance / (m_player->getSize().x / 2.0f);
		glm::vec2 newVelocity = m_ball->getBounceVelocity() * percentage * m_player->getStrength();
		newVelocity.y = std::abs(m_ball->getVelocity().y);
		m_ball->setVelocity(glm::normalize(newVelocity) * glm::length(m_ball->getVelocity()));
	}

	for (auto &brick : m_currentLevel.getBricks()) {
		if (brick.getCurrentHardnessPoints() == 0)
			continue;
		Collision collision = checkCollision(*m_ball, brick);
		if (std::get<0>(collision)) {
			Direction dir = std::get<1>(collision);
			glm::vec2 diffVector = std::get<2>(collision);
			_calcBallNewPositionAndVelocity(dir, diffVector);
			
			if (brick.isDestroyable()) {
				brick.doDamage(m_ball->getDamage());
			}

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
}

void Game::resetBall() {
	m_ball->setStuck(true);
	glm::vec2 ballPosition = m_player->getPosition() + glm::vec2(m_player->getSize().x / 2, m_ball->getSize().y);
	m_ball->resetPosition(ballPosition);
}

void Game::resetPlayer() {
	m_player->resetPosition(PLAYER_START_POSITION);
}

// AABB
Collision Game::checkCollision(GameObject &a, GameObject &b) {
	bool xCollision = a.getPosition().x + a.getSize().x >= b.getPosition().x &&
		b.getPosition().x + b.getSize().x >= a.getPosition().x;
	bool yCollision = a.getPosition().y + a.getSize().y >= b.getPosition().y &&
		b.getPosition().y + b.getSize().y >= a.getPosition().y;
	return std::tuple(xCollision && yCollision, Direction::UP, glm::vec2(0.0f));
}

Collision Game::checkCollision(Ball &ball, GameObject &gameObject) {
	glm::vec2 aabbHalf = glm::vec2(gameObject.getSize().x / 2, gameObject.getSize().y / 2);
	glm::vec2 aabbCenter = glm::vec2(gameObject.getPosition().x + aabbHalf.x, gameObject.getPosition().y + aabbHalf.y);
	
	glm::vec2 ballCenter = ball.getPosition() + ball.getRadius();

	glm::vec2 difference = ballCenter - aabbCenter;

	glm::vec2 clamped = glm::clamp(difference, -aabbHalf, aabbHalf);

	glm::vec2 closestPoint = aabbCenter + clamped;

	difference = closestPoint - ballCenter;
	if (glm::length(difference) < ball.getRadius()) {
		return std::tuple(true, _getDirection(difference), difference);
	}
	return std::tuple(false, Direction::UP, glm::vec2(0.0f));
}
