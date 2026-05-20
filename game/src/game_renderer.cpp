#include "game_renderer.hpp"

void GameRenderer::_bindShader(ShaderView shader) {
    if (m_activeShader != shader) {
        m_activeShader = shader;
        shader->use();
    }
}

GameRenderer::GameRenderer(SpriteRendererPtr spriteRenderer) : m_spriteRenderer(std::move(spriteRenderer)) {}

void GameRenderer::submit(const SpriteCommand &command) {
    m_renderQueue.push(command);
}

void GameRenderer::flush() {
    m_renderQueue.sort();

    for (const auto &cmd : m_renderQueue.getCommands()) {
        _bindShader(cmd.shader);
        m_spriteRenderer->drawSprite(*cmd.shader, *cmd.texture, cmd.position, cmd.size, cmd.rotation, cmd.color);
    }

    m_particleShader->use();
    for (const auto &emitter : m_particleEmitters) {
        emitter->render(*m_particleShader);
    }

    m_renderQueue.clear();
    m_activeShader = nullptr;
}

void GameRenderer::setParticleShader(ShaderPtr shader) {
    m_particleShader = shader;
}

void GameRenderer::addParticleEmitter(ParticleEmitterPtr emitter) {
    m_particleEmitters.push_back(emitter);
}
