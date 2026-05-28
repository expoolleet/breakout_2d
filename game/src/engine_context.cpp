#include "engine_context.hpp"

#include "logging.hpp"

EngineContext::EngineContext() {
    pathManager = std::make_unique<PathManager>();
    pathManager->init();

    audioManager = std::make_unique<AudioManager>(pathManager.get());
    audioManager->init();

    textureManager = std::make_unique<TextureManager>(pathManager.get());

    eventDispatcher = std::make_unique<EventDispatcher>();

    logging::Log("Engine context is created");
}

EngineContext::~EngineContext() noexcept {
    eventDispatcher->clear();
}
