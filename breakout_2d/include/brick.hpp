#pragma once

#include "brick_type.hpp"
#include "collision_type.hpp"
#include "game_object.hpp"

#include <glm/glm.hpp>

class Texture2D; // fwd

class Brick : public GameObject {
private:
	BrickType m_type = BrickType::BRICK_NONE;
	int m_hardnessPoints = 0;
	int m_maxHardnessPoints = 0;
public:
	glm::vec3 Color = glm::vec3(1.0f);
	Brick(const Texture2D &texture, glm::vec2 position, glm::vec2 size, BrickType type);
	void update(float dt) override;
	void fixedUpdate(float dt) override;
	Collision checkCollision(GameObject &gameObject) override;

	BrickType getType() const;
	int getCurrentHardnessPoints() const;
	int getMaxHardnessPoints() const;
	void doDamage(unsigned int damage);
	void doHeal(unsigned int heal);
	void reset() override;
};