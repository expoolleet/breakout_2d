#include "particle_emitter.hpp"

#include <format>
#include <glm/glm.hpp>

#include "fast_random.hpp"
#include "glad/glad.h"
#include "logging.hpp"
#include "particle.hpp"
#include "render_type.hpp"
#include "shader.hpp"
#include "texture_2d.hpp"

int ParticleEmitter::_findFirstUnusedParticle() {
    for (int i = m_lastUnusedParticle; i < m_particleCount; ++i) {
        if (m_particlePool[i].lifeTime <= 0.0f) {
            m_lastUnusedParticle = i;
            return i;
        }
    }
    for (int i = 0; i < m_particleCount; ++i) {
        if (m_particlePool[i].lifeTime <= 0.0f) {
            m_lastUnusedParticle = i;
            return i;
        }
    }
    logging::Warn(
        "Particle emitter was cycled through all {} particles from pool and did not find the "
        "unused one. Increase particle limit or decrease particle life time",
        m_particleCount);
    m_lastUnusedParticle = (m_lastUnusedParticle + 1) % m_particleCount;
    return m_lastUnusedParticle;
}

void ParticleEmitter::_fillPool() {
    m_particlePool.clear();
    m_particlePool.reserve(m_particleCount);
    for (int i = 0; i < m_particleCount; ++i) {
        m_particlePool.push_back(Particle());
    }
}

ParticleEmitter::ParticleEmitter(Texture2DPtr texture, int count) : m_texture(texture), m_particleCount(count) {}

void ParticleEmitter::init() {
    if (m_initialized) return;
    m_initialized = true;

    float vertices[16] = {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    int indices[6] = {0, 1, 2, 2, 3, 0};

    if (render::type == RenderType::OpenGL) {
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

        glGenBuffers(1, &m_particleVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, position));
        glVertexAttribDivisor(1, 1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, color));
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, scale));
        glVertexAttribDivisor(3, 1);

        glBindVertexArray(0);

        _fillPool();
    }
}

void ParticleEmitter::prepare(SpriteObject &object, int newParticles, glm::vec2 offset, bool overrideColor) {
    if (!m_emitWhenStanding && glm::length(object.getPosition() - m_objectPositionMap[&object]) < 0.001f) {
        return;
    }
    m_objectPositionMap[&object] = object.getPosition();
    for (int i = 0; i < newParticles; ++i) {
        respawnParticleAtObject(m_particlePool[_findFirstUnusedParticle()], object, offset, overrideColor);
    }
}

void ParticleEmitter::prepareAtPosition(glm::vec2 position, int newParticles) {
    for (int i = 0; i < newParticles; ++i) {
        respawnParticle(m_particlePool[_findFirstUnusedParticle()], position);
    }
}

void ParticleEmitter::update(float dt) {
    for (int i = 0; i < m_particleCount; ++i) {
        Particle &particle = m_particlePool[i];
        if (particle.lifeTime <= 0.0f) continue;
        particle.lifeTime -= dt;
        particle.velocity += (m_gravityEnabled ? glm::vec2(0.0f, GRAVITATIONAL_ACCELERATION) * dt : glm::vec2(0.0f));
        particle.position -= particle.velocity * dt;
        particle.color.a = particle.lifeTime / m_particleLifeTime;
        particle.scale = m_particleScale * particle.color.a;
    }
}

void ParticleEmitter::render(Shader &shader) {
    auto partitionedParticles =
        std::partition(m_particlePool.begin(), m_particlePool.end(), [](const Particle &p) { return p.lifeTime <= 0.0f; });
    size_t activeParticleCount = std::distance(partitionedParticles, m_particlePool.end());
    if (activeParticleCount == 0) return;
    m_texture->bind();
    if (render::type == RenderType::OpenGL) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);  // buffer orphaning
        glBufferSubData(GL_ARRAY_BUFFER, 0, activeParticleCount * sizeof(Particle), &(*partitionedParticles));

        glBindVertexArray(m_VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, activeParticleCount);
        glBindVertexArray(0);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void ParticleEmitter::respawnParticleAtObject(Particle &particle, SpriteObject &object, glm::vec2 offset, bool overrideColor) {
    particle.lifeTime = m_particleLifeTime;
    float randomPosX = fastrand::frandomFloatInRange(m_positionOffsetRange.first, m_positionOffsetRange.second);
    float randomPosY = fastrand::frandomFloatInRange(m_positionOffsetRange.first, m_positionOffsetRange.second);
    particle.position = object.getPosition() + offset + glm::vec2(randomPosX, randomPosY);
    float randomVelX = fastrand::frandomFloatInRange(m_velocityOffsetRange.first, m_velocityOffsetRange.second);
    float randomVelY = fastrand::frandomFloatInRange(m_velocityOffsetRange.first, m_velocityOffsetRange.second);
    particle.velocity = object.getVelocity() * (1.0f - m_particleDelay) + glm::vec2(randomVelX, randomVelY);
    float randomBrightness = fastrand::frandomFloatInRange(0.3f, 1.0f);
    particle.color =
        (overrideColor ? object.getColor() : m_particleColor) * glm::vec4(randomBrightness, randomBrightness, randomBrightness, 1.0f);
    particle.scale = m_particleScale;
}

void ParticleEmitter::respawnParticle(Particle &particle, glm::vec2 position) {
    particle.lifeTime = m_particleLifeTime;
    float randomPosX = fastrand::frandomFloatInRange(m_positionOffsetRange.first, m_positionOffsetRange.second);
    float randomPosY = fastrand::frandomFloatInRange(m_positionOffsetRange.first, m_positionOffsetRange.second);
    particle.position = position + glm::vec2(randomPosX, randomPosY);
    float randomVelX = fastrand::frandomFloatInRange(m_velocityOffsetRange.first, m_velocityOffsetRange.second);
    float randomVelY = fastrand::frandomFloatInRange(m_velocityOffsetRange.first, m_velocityOffsetRange.second);
    particle.velocity = glm::vec2(randomVelX, randomVelY);
    float randomBrightness = fastrand::frandomFloatInRange(0.5f, 1.0f);
    particle.color = m_particleColor * glm::vec4(randomBrightness, randomBrightness, randomBrightness, 1.0f);
    particle.scale = m_particleScale;
}

void ParticleEmitter::setParticleLifeTime(float time) {
    m_particleLifeTime = time;
}

float ParticleEmitter::getParticleLifeTime() const noexcept {
    return m_particleLifeTime;
}

void ParticleEmitter::setParticleDelay(float delay) {
    m_particleDelay = delay;
}

float ParticleEmitter::getParticleDelay() const noexcept {
    return m_particleDelay;
}

void ParticleEmitter::setParticleScale(float scale) {
    m_particleScale = scale;
}

float ParticleEmitter::getParticleScale() const noexcept {
    return m_particleScale;
}

void ParticleEmitter::setNewParticleCount(int count) {
    m_particleCount = count;
    _fillPool();
}

int ParticleEmitter::getParticleCount() const noexcept {
    return m_particleCount;
}

void ParticleEmitter::setVelocityRandomOffsetRange(float a, float b) {
    if (a > b) {
        logging::Warn("Could not set range for velocity random offset ({} > {})", a, b);
    }
    m_velocityOffsetRange = {a, b};
}

void ParticleEmitter::setPositionRandomOffsetRange(float a, float b) {
    if (a > b) {
        logging::Warn("Could not set range for position random offset ({} > {})", a, b);
    }
    m_positionOffsetRange = {a, b};
}

void ParticleEmitter::setEmitWhenStandingEnabled(bool enabled) {
    m_emitWhenStanding = enabled;
}

void ParticleEmitter::setGravityEnabled(bool enabled) {
    m_gravityEnabled = enabled;
}

void ParticleEmitter::setParticleColor(glm::vec4 color) {
    m_particleColor = color;
}
