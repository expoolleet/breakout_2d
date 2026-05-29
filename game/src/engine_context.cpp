#include "engine_context.hpp"

#include "logging.hpp"

EngineContext::EngineContext() : m_audioManager(&m_pathManager), m_textureManager(&m_pathManager) {
    m_pathManager.init();
    m_audioManager.init();
    logging::Log("Engine context is created");
}

EngineContext::~EngineContext() noexcept {
    m_eventDispatcher.clear();
    logging::Log("Engine context is destroyed properly");
}

AudioManager &EngineContext::getAudioManager() noexcept {
    return m_audioManager;
}

EventDispatcher &EngineContext::getEventDispatcher() noexcept {
    return m_eventDispatcher;
}

PathManager &EngineContext::getPathManager() noexcept {
    return m_pathManager;
}

TextureManager &EngineContext::getTextureManager() noexcept {
    return m_textureManager;
}
