#pragma once

#include "game_object.hpp"
#include "collision_type.hpp"

#include <glm/glm.hpp>

class Texture2D; // fwd
class Player;

class Ball : public GameObject {
private:
	bool m_stuck = true;
	float m_radius = 0.0;
	unsigned int m_damage = 1;
	glm::vec2 m_bounceVelocity = glm::vec2(0.3f, 1.0f);
	const Player *m_player = nullptr;

public:
	Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size, const Player &player);

	void update(float dt) override;
	void fixedUpdate(float dt) override;
	Collision checkCollision(GameObject &gameObject) override;

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