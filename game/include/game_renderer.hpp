#pragma once

#include <memory>

#include "particle_emitter.hpp"
#include "render_queue.hpp"
#include "shader.hpp"
#include "sprite_renderer.hpp"

class GameRenderer {
   private:
    std::vector<ParticleEmitterPtr> m_particleEmitters;
    SpriteRendererPtr m_spriteRenderer;
    ShaderPtr m_spriteShader;
    ShaderPtr m_particleShader;
    RenderQueue m_renderQueue;
    ShaderView m_activeShader = nullptr;

    void _bindShader(ShaderView shader);

   public:
    GameRenderer(SpriteRendererPtr spriteRenderer);

    void submit(const SpriteCommand &command);
    void flush();
    void setParticleShader(ShaderPtr shader);
    void addParticleEmitter(ParticleEmitterPtr emitter);
};
