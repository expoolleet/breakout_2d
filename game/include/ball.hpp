#pragma once

#include <glm/glm.hpp>

#include "collision_type.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "texture_2d.hpp"

constexpr float BALL_DEFAULT_SPEED = 20.0f;
constexpr glm::vec2 BALL_DEFAULT_SIZE = glm::vec2(0.65f);

class Ball : public GameObject {
   private:
    glm::vec2 m_bounceVelocity = glm::vec2(0.3f, 1.0f);
    glm::vec2 m_stuckPosition = glm::vec2(0.0f);
    float m_radius = 0.0;
    unsigned int m_damage = 1;
    bool m_stuck = false;
    PlayerPtr m_player;

   public:
    Ball(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);
    Ball(ContextPtr context, Texture2DPtr texture);

    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    void reset() override;

    bool isStuck() const noexcept;
    void setStuck(bool state);

    float getRadius() const noexcept;
    void setRadius(float radius);

    unsigned int getDamage() const noexcept;
    void setDamage(unsigned int damage);

    glm::vec2 getBounceVelocity() const noexcept;
    void setBounceVelocity(glm::vec2 velocity);

    void setStuckLocalPosition(glm::vec2 stuckPosition);

    void setPlayer(PlayerPtr player);
};

using BallPtr = observer_ptr<Ball>;
