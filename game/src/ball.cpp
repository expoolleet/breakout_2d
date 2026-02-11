#include "ball.hpp"
#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "event_dispatcher.hpp"
#include "event_type.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "texture_2d.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

Ball::Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size, const Player &player) : GameObject(texture, position, size), m_player(&player) { }

void Ball::move(float dt, float windowWidth, float windowHeight) {
	m_previousPosition = m_position;
	m_position += m_velocity * m_speed * dt;
	if (m_position.x <= 0.0f) {
		m_position.x = 0.0f;
		m_velocity.x = -m_velocity.x;
	}
	if (m_position.x + m_size.x >= windowWidth) {
		m_position.x = windowWidth - m_size.x;
		m_velocity.x = -m_velocity.x;
	}
	if (m_position.y + m_size.y >= windowHeight) {
		m_position.y = windowHeight - m_size.y;
		m_velocity.y = -m_velocity.y;
	}
}

void Ball::update(float dt) {

}

void Ball::fixedUpdate(float dt) {
	if (isStuck()) {
		glm::vec2 ballPosition = m_player->getPosition() + glm::vec2(m_player->getSize().x / 2, m_size.y);
		setPosition(ballPosition);
	}
	else {
		move(dt, static_cast<float>(Window::getWidth()), static_cast<float>(Window::getHeight()));
	}

	if (m_position.y <= 0.0f) {
		EventDispatcher::Get().emit(BallFliedOff { *this });
	}
}

Collision Ball::checkCollision(GameObject &gameObject) {
	if (isStuck())
		return _cd::NoneCollision;
	if (Player *player = dynamic_cast<Player *>(&gameObject)) {
		Collision playerCollision = _cd::checkCollision(*this, *player);
		if (std::get<0>(playerCollision)) {
			float centerBoard = m_player->getPosition().x + m_player->getSize().x / 2;
			float distance = (getPosition().x + getRadius()) - centerBoard;
			float percentage = distance / (m_player->getSize().x / 2.0f);
			glm::vec2 newVelocity = getBounceVelocity() * percentage * player->getStrength();
			newVelocity.y = std::abs(getVelocity().y);
			setVelocity(glm::normalize(newVelocity) * glm::length(getVelocity()));
		}
	}
	return _cd::NoneCollision;
}

void Ball::reset() {
	GameObject::reset();
	setStuck(true);
}

bool Ball::isStuck() {
	return m_stuck;
}

void Ball::setStuck(bool state) {
	m_stuck = state;
}

void Ball::setRadius(float radius) {
	m_radius = radius;
}

float Ball::getRadius() {
	return m_radius;
}

void Ball::setDamage(unsigned int damage) {
	m_damage = damage;
}

unsigned int Ball::getDamage() {
	return m_damage;
}

glm::vec2 Ball::getBounceVelocity() {
	return m_bounceVelocity;
}

void Ball::setBounceVelocity(glm::vec2 velocity) {
	m_bounceVelocity = velocity;
}
