#include "powerup_factory.hpp"

#include "powerup.hpp"
#include "texture_literals.hpp"

using namespace texture_literals;

PowerUpFactory::PowerUpFactory(PowerUpFactoryCreateInfo createInfo)
    : m_context(createInfo.contextPtr), m_objectManager(createInfo.objectManagerPtr) {}

PowerUpPtr PowerUpFactory::createPowerUp(PowerUpType type, glm::vec2 position) {
    PowerUpData data = getPowerUpData(type);
    return m_objectManager->create<PowerUp>(data.texture, type, data.color, data.duration, position, DEFAULT_POWERUP_SIZE);
}

PowerUpData PowerUpFactory::getPowerUpData(PowerUpType type) {
    TextureManager &textureManager = m_context->getTextureManager();
    switch (type) {
        case PowerUpType::FastHeroBall:
            return PowerUpData(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), 10.0f, textureManager.getTexture(BRICK_TEXTURE));
        case PowerUpType::StickyPlayer:
            return PowerUpData(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), 10.0f, textureManager.getTexture(BRICK_TEXTURE));
        case PowerUpType::PassTrough:
            return PowerUpData(glm::vec4(0.25f, 0.75f, 0.0f, 1.0f), 3.0f, textureManager.getTexture(BRICK_TEXTURE));
        case PowerUpType::WidePlayer:
            return PowerUpData(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 10.0f, textureManager.getTexture(BRICK_TEXTURE));
        case PowerUpType::None:
        default:
            return PowerUpData();
    }
}
