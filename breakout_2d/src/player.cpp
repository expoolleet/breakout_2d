#include "pch.hpp"

#include "player.hpp"
#include "game_object.hpp"

#include <glm/glm.hpp>

class Texture2D; // fwd

Player::Player(const Texture2D &texture, glm::vec2 position, glm::vec2 size) : GameObject(texture, position, size) {  }

void Player::setStrength(float strength) {
	m_strength = strength;
}

float Player::getStrength() {
	return m_strength;
}
