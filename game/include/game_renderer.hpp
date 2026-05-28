#pragma once

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
    ShaderPtr m_activeShader;

    void _bindShader(ShaderPtr shader);

   public:
    GameRenderer(SpriteRendererPtr spriteRenderer, ShaderPtr shader);

    void submit(const SpriteCommand &command);
    void flush();
    void setParticleShader(ShaderPtr shader);
    void addParticleEmitter(ParticleEmitterPtr emitter);
};

using GameRendererPtr = observer_ptr<GameRenderer>;
