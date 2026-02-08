#include "pch.hpp"

#include "game_object.hpp"
#include "player.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

class Texture2D; // fwd

Player::Player(const Texture2D &texture, glm::vec2 position, glm::vec2 size) : GameObject(texture, position, size) {  }

void Player::update(float dt) {
}

void Player::fixedUpdate(float dt) {
	float offset = 5.0f;
	glm::vec2 playerPosition = getPosition();
	playerPosition += getVelocity() * getSpeed() * dt;
	int leftSide = 0.0f + offset;
	if (playerPosition.x <= leftSide) {
		playerPosition.x = leftSide;
	}
	float rightSide = Window::getWidth() - getSize().x - offset;
	if (playerPosition.x >= rightSide) {
		playerPosition.x = rightSide;
	}
	setPosition(playerPosition);
}

void Player::setStrength(float strength) {
	m_strength = strength;
}

float Player::getStrength() {
	return m_strength;
}
