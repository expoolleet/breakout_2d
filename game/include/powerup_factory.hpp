#pragma once

#include "object_manager.hpp"
#include "powerup.hpp"
#include "powerup_data.hpp"

struct PowerUpFactoryCreateInfo {
    ContextPtr contextPtr;
    ObjectManagerPtr objectManagerPtr;
};

// PowerUp factory
class PowerUpFactory {
   private:
    ContextPtr m_context;
    ObjectManagerPtr m_objectManager;

   public:
    PowerUpFactory(PowerUpFactoryCreateInfo createInfo);

    PowerUpPtr createPowerUp(PowerUpType type, glm::vec2 position);

    PowerUpData getPowerUpData(PowerUpType type);
};

using PowerUpFactoryPtr = observer_ptr<PowerUpFactory>;
