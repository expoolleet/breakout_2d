#include "pch.hpp"

#include "ball.hpp"
#include "game_object.hpp"
#include "texture_2d.hpp"

#include <glm/glm.hpp>

Ball::Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size) : GameObject(texture, position, size) { }

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
	if (m_position.y + m_size.y > windowHeight) {
		m_position.y = windowHeight - m_size.y;
		m_velocity.y = -m_velocity.y;
	}
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
