#pragma once

#include <glm/glm.hpp>

#include "brick_type.hpp"
#include "collision_type.hpp"
#include "game_object.hpp"
#include "powerup_type.hpp"
#include "texture_2d.hpp"

constexpr glm::vec2 BRICK_SIZE = glm::vec2(2.0f, 1.0f);

class Brick : public GameObject {
   private:
    BrickType m_brickType = BrickType::None;
    int m_hardnessPoints = 0;
    int m_maxHardnessPoints = 0;
    PowerUpType m_powerUp = PowerUpType::None;

   public:
    Brick(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size, BrickType type);
    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    BrickType getBrickType() const noexcept;
    PowerUpType getPowerUpType() const noexcept;
    void setPowerUpType(PowerUpType type);
    void setHardenessPoints(int hp) noexcept;
    int getCurrentHardnessPoints() const noexcept;
    int getMaxHardnessPoints() const noexcept;
    void damage(int damage);
    void heal(int heal);
    void reset() noexcept override;
};

using BrickPtr = observer_ptr<Brick>;
