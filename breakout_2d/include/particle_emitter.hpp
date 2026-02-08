#pragma once

#include "particle.hpp"

#include <random>
#include <vector>

// fwd;
class Texture2D;
class Shader;
class GameObject;

class ParticleEmitter {
private:
	std::vector<Particle> m_particlePool = { };
	const Texture2D *m_texture = nullptr;
	bool m_initialized = false;
	int m_lastUnusedParticle = 0;
	int m_particleLimit = 0;
	unsigned int m_VAO = 0;
	float m_particleAttenuationSpeed = 2.5f;
	float m_particleLifeTime = 0.0f;
	float m_particleDelay = 0.9f;
	float m_particleScale = 10.0f;

	int _findFirstUnusedParticle();

public:
	ParticleEmitter(const Texture2D &texture, int limit);
	~ParticleEmitter() = default;
	void init();
	void update(float dt, GameObject &gameObject, int newParticles, glm::vec2 offset = glm::vec2(0.0f));
	void emit(Shader &shader);
	void respawnParticle(Particle &particle, GameObject &gameObject, glm::vec2 offset = glm::vec2(0.0f));

	void setParticleAttenuationSpeed(float speed);
	float getParticleAttenuationSpeed();

	void setParticleLifeTime(float time);
	float getParticleLifeTime();

	void setParticleDelay(float delay);
	float getParticleDelay();

	void setParticleScale(float scale);
	float getParticleScale();

	void setParticleLimit(int limit);
	int getParticleLimit();
};