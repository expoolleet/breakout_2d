#include "pch.hpp"

#include "brick.hpp"
#include "brick_type.hpp"
#include "brick_data.hpp"
#include "game_object.hpp"

#include <algorithm>
#include <glm/glm.hpp>

Brick::Brick(const Texture2D &texture, glm::vec2 position, glm::vec2 size, BrickType type) : GameObject(texture, position, size), m_type(type) {
	BrickData data = getBrickData(type);
	m_isDestroyable = data.isDestroyable;
	m_hardnessPoints = data.maxHardnessPoints;
	m_maxHardnessPoints = data.maxHardnessPoints;
	Color = data.color;
}

int Brick::getCurrentHardnessPoints() const {
	return m_hardnessPoints;
}

int Brick::getMaxHardnessPoints() const {
	return m_maxHardnessPoints;
}

void Brick::doDamage(unsigned int damage) {
	m_hardnessPoints = std::max(0, m_hardnessPoints - static_cast<int>(damage));
	if (m_hardnessPoints == 0) {
		destroy();
	}
}

void Brick::doHeal(unsigned int heal) {
	m_hardnessPoints = std::min(m_maxHardnessPoints, m_hardnessPoints + static_cast<int>(heal));
}

void Brick::reset() {
	m_hardnessPoints = m_maxHardnessPoints;
	IsHidden = false;
}

BrickType Brick::getType() const {
	return m_type;
}
