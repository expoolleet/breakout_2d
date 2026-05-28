#pragma once

#include "audio_manager.hpp"
#include "custom_attributes.hpp"
#include "event_dispatcher.hpp"
#include "observer_ptr.hpp"
#include "path_manager.hpp"
#include "texture_manager.hpp"

class EngineContext {
   public:
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<EventDispatcher> eventDispatcher;
    std::unique_ptr<PathManager> pathManager;
    std::unique_ptr<TextureManager> textureManager;

    EngineContext();
    ~EngineContext() noexcept;

    EngineContext(const EngineContext &) = delete;
    EngineContext &operator=(const EngineContext &) = delete;
    EngineContext(EngineContext &&) = delete;
    EngineContext &operator=(EngineContext &&) = delete;
};

using Context = EngineContext;
using EngineContextPtr = observer_ptr<EngineContext>;
using ContextPtr = observer_ptr<Context>;
