#pragma once

#include <unordered_map>
#include <vector>

#include "game_object.hpp"
#include "particle.hpp"
#include "shader.hpp"
#include "texture_2d.hpp"

#define MAX_PARTICLES 7500

#define GRAVITATIONAL_ACCELERATION 9.807f

class ParticleEmitter {
   private:
    std::vector<Particle> m_particlePool = {};
    const Texture2D *m_texture = nullptr;
    bool m_initialized = false;
    bool m_gravityEnabled = false;
    bool m_emitWhenStanding = false;
    int m_lastUnusedParticle = 0;
    int m_particleCount = 0;
    unsigned int m_VAO = 0;
    unsigned int m_particle_VBO = 0;
    float m_particleLifeTime = 0.0f;
    float m_particleDelay = 0.9f;
    float m_particleScale = 10.0f;
    std::pair<float, float> m_velocityOffsetRange = {-1.0f, 1.0f};
    std::pair<float, float> m_positionOffsetRange = {0.0f, 0.0f};
    glm::vec4 m_particleColor = glm::vec4(1.0f);

    std::unordered_map<GameObject *, glm::vec2> m_objectPositionMap;

    int _findFirstUnusedParticle();
    void _fillPool();

   public:
    ParticleEmitter(const Texture2D &texture, int count);
    ~ParticleEmitter() = default;
    void init();
    void prepare(GameObject &gameObject, int newParticles, glm::vec2 offset = glm::vec2(0.0f), bool overrideColor = false);
    void prepareAtPosition(glm::vec2 position, int newParticles);
    void update(float dt);
    void render(Shader &shader);
    void respawnParticleAtObject(Particle &particle, GameObject &gameObject, glm::vec2 offset = glm::vec2(0.0f),
                                 bool overrideColor = false);
    void respawnParticle(Particle &particle, glm::vec2 position);

    float getParticleLifeTime() const noexcept;
    void setParticleLifeTime(float time);

    float getParticleDelay() const noexcept;
    void setParticleDelay(float delay);

    float getParticleScale() const noexcept;
    void setParticleScale(float scale);

    int getParticleCount() const noexcept;
    void setNewParticleCount(int count);

    void setVelocityRandomOffsetRange(float a, float b);
    void setPositionRandomOffsetRange(float a, float b);

    void setEmitWhenStanding(bool flag);

    void setGravityEnabled(bool flag);

    void setParticleColor(glm::vec4 color);
};
