#pragma once

#include "audio_manager.hpp"
#include "custom_attributes.hpp"
#include "event_dispatcher.hpp"
#include "observer_ptr.hpp"
#include "path_manager.hpp"
#include "texture_manager.hpp"

class EngineContext {
   private:
    AudioManager m_audioManager;
    EventDispatcher m_eventDispatcher;
    PathManager m_pathManager;
    TextureManager m_textureManager;

   public:
    EngineContext();
    ~EngineContext() noexcept;

    EngineContext(const EngineContext &) = delete;
    EngineContext &operator=(const EngineContext &) = delete;
    EngineContext(EngineContext &&) = delete;
    EngineContext &operator=(EngineContext &&) = delete;

    AudioManager &getAudioManager() noexcept;
    EventDispatcher &getEventDispatcher() noexcept;
    PathManager &getPathManager() noexcept;
    TextureManager &getTextureManager() noexcept;
};

using Context = EngineContext;
using EngineContextPtr = observer_ptr<EngineContext>;
using ContextPtr = observer_ptr<Context>;
