#pragma once

#include <memory>

#include "particle_emitter.hpp"
#include "render_queue.hpp"
#include "shader.hpp"
#include "sprite_renderer.hpp"

using ParticleEmitterPtr = std::unique_ptr<ParticleEmitter>;
using SpriteRendererPtr = std::unique_ptr<SpriteRenderer>;
using ShaderPtr = std::shared_ptr<Shader>;

class GameRenderer {
   public:
    GameRenderer(SpriteRendererPtr spriteRenderer, ShaderPtr spriteShader, ShaderPtr particleShader);

    void submit(const SpriteCommand &command);
    void flush();
    void addParticleEmitter(ParticleEmitterPtr emitter);

   private:
    std::vector<ParticleEmitterPtr> m_particleEmitters;
    SpriteRendererPtr m_spriteRenderer;
    ShaderPtr m_spriteShader;
    ShaderPtr m_particleShader;
    RenderQueue m_renderQueue;
    unsigned int m_bindedShaderID = 0;
};
