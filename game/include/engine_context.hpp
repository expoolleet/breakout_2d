#pragma once

#include "audio_manager.hpp"
#include "custom_attributes.hpp"
#include "event_dispatcher.hpp"
#include "memory"
#include "path_manager.hpp"
#include "shader_observer.hpp"
#include "texture_manager.hpp"

class EngineContext {
   private:
    EngineContext();
    ~EngineContext() noexcept;

   public:
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<EventDispatcher> eventDispatcher;
    std::unique_ptr<PathManager> pathManager;
    std::unique_ptr<ShaderObserver> shaderObserver;
    std::unique_ptr<TextureManager> textureManager;

    EngineContext(const EngineContext &) = delete;
    EngineContext &operator=(const EngineContext &) = delete;
    EngineContext(EngineContext &&) = delete;
    EngineContext &operator=(EngineContext &&) = delete;

    static EngineContext &get() noexcept;
};

using Context = EngineContext;
