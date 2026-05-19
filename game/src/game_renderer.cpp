#include "game_renderer.hpp"

GameRenderer::GameRenderer(SpriteRendererPtr spriteRenderer, ShaderPtr spriteShader, ShaderPtr particleShader)
    : m_spriteRenderer(std::move(spriteRenderer)), m_spriteShader(spriteShader), m_particleShader(particleShader) {}

void GameRenderer::submit(const SpriteCommand &command) {
    m_renderQueue.push(command);
}

void GameRenderer::flush() {
    m_renderQueue.sort();

    for (const auto &cmd : m_renderQueue.getCommands()) {
        if (m_bindedShaderID != cmd.shader->getID()) {
            m_bindedShaderID = cmd.shader->getID();
            cmd.shader->use();
        }
        m_spriteRenderer->drawSprite(*cmd.shader, *cmd.texture, cmd.position, cmd.size, cmd.rotation, cmd.color);
    }

    m_particleShader->use();
    for (const auto &emitter : m_particleEmitters) {
        emitter->render(*m_particleShader);
    }
}

void GameRenderer::addParticleEmitter(ParticleEmitterPtr emitter) {
    m_particleEmitters.push_back(std::move(emitter));
}
