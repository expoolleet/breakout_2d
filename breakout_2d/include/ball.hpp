#pragma once

#include "game_object.hpp"

#include <glm/glm.hpp>

class Texture2D; // fwd

class Ball : public GameObject {
private:
	bool m_stuck = true;
	float m_radius = 0.0;
	unsigned int m_damage = 1;
	glm::vec2 m_bounceVelocity = glm::vec2(0.3f, 1.0f);

public:
	Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size);
	void move(float dt, float windowWidth, float windowHeight);

	bool isStuck();
	void setStuck(bool state);
	
	void setRadius(float radius);
	float getRadius();

	void setDamage(unsigned int damage);
	unsigned int getDamage();

	glm::vec2 getBounceVelocity();
	void setBounceVelocity(glm::vec2 velocity);
};