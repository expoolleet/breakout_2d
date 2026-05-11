#include "engine_context.hpp"

#include "logging.hpp"

EngineContext::EngineContext() {
    logging::Log("Engine context is created");
    audioManager = std::make_unique<AudioManager>();
    audioManager->init();

    eventDispatcher = std::make_unique<EventDispatcher>();

    pathManager = std::make_unique<PathManager>();
    pathManager->init();

    shaderObserver = std::make_unique<ShaderObserver>();
    shaderObserver->startObserving();

    textureManager = std::make_unique<TextureManager>();
}

EngineContext::~EngineContext() noexcept {
    eventDispatcher->clear();
    shaderObserver->stopObserving();
}

EngineContext &EngineContext::get() noexcept {
    NO_DESTROY_ATTR static EngineContext context;
    return context;
}
