#include "game_object.hpp"
#include "logging.hpp"
#include "particle.hpp"
#include "particle_emitter.hpp"
#include "shader.hpp"
#include "texture_2d.hpp"
#include "fast_random.hpp"

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <format>

#define MAX_PARTICLE_COUNT 5000

int ParticleEmitter::_findFirstUnusedParticle() {
	for (int i = m_lastUnusedParticle; i < m_particleLimit; ++i) {
		if (m_particlePool[i].lifeTime <= 0.0f) {
			m_lastUnusedParticle = i;
			return i;
		}
	}
	for (int i = 0; i < m_particleLimit; ++i) {
		if (m_particlePool[i].lifeTime <= 0.0f) {
			m_lastUnusedParticle = i;
			return i;
		}
	}
	Logging::Warn(std::format("Particle emitter was cycled through all {} particles from pool and did not find the unused one. Increase particle limit or decrease particle life time", m_particleLimit));
	m_lastUnusedParticle = 0;
	return 0;
}

ParticleEmitter::ParticleEmitter(const Texture2D &texture, int limit) : m_texture(&texture), m_particleLimit(limit) { }

void ParticleEmitter::init() {
	if (m_initialized)
		return;
	m_initialized = true;

	float vertices[16] = {
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	int indices[6] = { 0, 1, 2, 2, 3, 0 };

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	unsigned int VBO, EBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(0));
	glBindVertexArray(0);

	m_particlePool.reserve(MAX_PARTICLE_COUNT);
	for (int i = 0; i < MAX_PARTICLE_COUNT; ++i) {
		m_particlePool.push_back(Particle());
	}
}

void ParticleEmitter::update(float dt, GameObject &gameObject, int newParticles, glm::vec2 offset) {
	for (int i = 0; i < newParticles; ++i) {
		int unusedParticleIndex = _findFirstUnusedParticle();
		respawnParticle(m_particlePool[unusedParticleIndex], gameObject, offset);
	}
	for (int i = 0; i < m_particleLimit; ++i) {
		Particle &particle = m_particlePool[i];
		particle.lifeTime -= dt;
		if (particle.lifeTime <= 0.0f)
			continue;
		particle.position -= particle.velocity * dt;
		particle.color.a -= m_particleAttenuationSpeed * dt;
	}
}

void ParticleEmitter::emit(Shader &shader) {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (int i = 0; i < m_particleLimit; ++i) {
		Particle &particle = m_particlePool[i];
		if (particle.lifeTime <= 0.0f)
			continue;
		shader.setVec4("color", particle.color);
		shader.setVec2("offset", particle.position);
		shader.setFloat("scale", m_particleScale);
		m_texture->bind();
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleEmitter::respawnParticle(Particle &particle, GameObject &gameObject, glm::vec2 offset) {
	particle.lifeTime = m_particleLifeTime;
	particle.position = gameObject.getPosition() + offset + _fr::randomFloatInRange(-5.0f, 5.0f);
	particle.velocity = gameObject.getVelocity() * (1.0f - m_particleDelay);
	float randomColor = _fr::randomFloatInRange(0.3f, 0.7f);
	particle.color = glm::vec4(randomColor, randomColor, randomColor, 1.0f);
}

void ParticleEmitter::setParticleAttenuationSpeed(float speed) {
	m_particleAttenuationSpeed = speed;
}

float ParticleEmitter::getParticleAttenuationSpeed() {
	return m_particleAttenuationSpeed;
}

void ParticleEmitter::setParticleLifeTime(float time) {
	m_particleLifeTime = time;
}

float ParticleEmitter::getParticleLifeTime() {
	return m_particleLifeTime;
}

void ParticleEmitter::setParticleDelay(float delay) {
	m_particleDelay = delay;
}

float ParticleEmitter::getParticleDelay() {
	return m_particleDelay;
}

void ParticleEmitter::setParticleScale(float scale) {
	m_particleScale = scale;
}

float ParticleEmitter::getParticleScale() {
	return m_particleScale;
}

void ParticleEmitter::setParticleLimit(int limit) {
	if (limit > MAX_PARTICLE_COUNT) {
		Logging::Warn(std::format("Maximum particle count is {}, can not adjust limit with {}", MAX_PARTICLE_COUNT, limit));
		return;
	}
	m_lastUnusedParticle = 0;
	m_particleLimit = limit;
}

int ParticleEmitter::getParticleLimit() {
	return m_particleLimit;
}
