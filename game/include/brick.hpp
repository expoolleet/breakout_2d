#pragma once

#include "brick_type.hpp"
#include "collision_type.hpp"
#include "game_object.hpp"
#include "powerup_type.hpp"

#include <glm/glm.hpp>

class Texture2D; // fwd

class Brick : public GameObject {
  private:
    BrickType m_brickType = BrickType::None;
    int m_hardnessPoints = 0;
    int m_maxHardnessPoints = 0;
    PowerUpType m_powerUp = PowerUp_None;

  public:
    Brick(const Texture2D &texture, glm::vec2 position, glm::vec2 size, BrickType type);
    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    BrickType getBrickType() const;
    PowerUpType getPowerUpType() const;
    void setPowerUpType(PowerUpType type);
    int getCurrentHardnessPoints() const;
    int getMaxHardnessPoints() const;
    void doDamage(unsigned int damage);
    void doHeal(unsigned int heal);
    void reset() override;
};
