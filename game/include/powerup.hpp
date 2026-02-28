#pragma once

#include "game_object.hpp"
#include "powerup_type.hpp"
#include "texture_2d.hpp"

#define DEFAULT_POWERUP_SIZE glm::vec2(2.0f, 0.5f)

class PowerUp : public GameObject {
   private:
    PowerUpType m_powerUpType = PowerUp_None;
    bool m_activated = false;
    float m_duration = 0.0f;

   public:
    PowerUp(PowerUpType type, glm::vec4 color, float duration, const Texture2D &texture, glm::vec2 position, glm::vec2 size);
    PowerUp();

    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    bool isActivated();
    bool isFinished();
    void activate();
    PowerUpType getType() const;
};
