#include "game_renderer.hpp"

void GameRenderer::_bindShader(ShaderView shader) {
    if (shader) {
        m_activeShader = shader;
    } else {
        m_activeShader = m_spriteShader.get();
    }
    m_activeShader->use();
}

GameRenderer::GameRenderer(SpriteRendererPtr spriteRenderer, ShaderPtr shader)
    : m_spriteRenderer(std::move(spriteRenderer)), m_spriteShader(shader) {}

void GameRenderer::submit(const SpriteCommand &command) {
    m_renderQueue.push(command);
}

void GameRenderer::flush() {
    m_renderQueue.sort();

    for (const auto &cmd : m_renderQueue.getCommands()) {
        _bindShader(cmd.shader);
        m_spriteRenderer->drawSprite(*m_activeShader, *cmd.texture, cmd.position, cmd.size, cmd.rotation, cmd.color);
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
