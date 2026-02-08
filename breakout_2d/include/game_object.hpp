#pragma once

#include <glm/glm.hpp>

#include "collision_type.hpp"

class Texture2D; // fwd

class GameObject {
protected:
	bool m_isDestroyable = false;
	bool m_isDead = false;
	glm::vec2 m_position = glm::vec3(0.0f);
	glm::vec2 m_previousPosition = glm::vec3(0.0f);
	glm::vec2 m_size = glm::vec3(0.0f);
	glm::vec2 m_velocity = glm::vec2(0.0f);
	glm::vec2 m_acceleration = glm::vec2(0.0f);
	float m_accelerationAttenuation = 0.1f;
	float m_speed = 0.0f;
	bool m_isHidden = false;
public:
	const Texture2D *Texture = nullptr;

	GameObject(const Texture2D &texture, glm::vec2 position, glm::vec2 size);
	GameObject(const Texture2D &texture);
	virtual ~GameObject() = default;

	virtual void update(float dt) = 0;
	virtual void fixedUpdate(float dt) = 0;
	virtual Collision checkCollision(GameObject &gameObject);

	virtual bool isDestroyable() const;
	virtual bool isDead() const;
	virtual void setDestructibility(bool flag);
	virtual void destroy();
	virtual void reset();

	virtual bool IsHidden() const;
	virtual void Hide(bool state);

	virtual glm::vec2 getPosition() const;
	virtual glm::vec2 getPreviousPosition();
	virtual void setPosition(glm::vec2 position);
	virtual void resetPosition(glm::vec2 position);

	virtual glm::vec2 getSize() const;
	virtual void setSize(glm::vec2 size);

	virtual glm::vec2 getVelocity() const;
	virtual void setVelocity(glm::vec2 velocity);

	virtual float getSpeed() const;
	virtual void setSpeed(float speed);
};