#pragma once

#include <glm/glm.hpp>

#include "collision_type.hpp"
#include "game_object.hpp"
#include "texture_2d.hpp"

inline constexpr int BALL_DAMAGE_MIN = 1;
inline constexpr int BALL_DAMAGE_MAX = 3;
inline constexpr float BALL_DEFAULT_SPEED = 20.0f;
inline constexpr glm::vec2 BALL_DEFAULT_SIZE = glm::vec2(0.65f);
inline constexpr glm::vec2 BALL_INITIAL_VELOCITY = glm::vec2(0.0f, 1.0f);

class Ball : public GameObject {
   private:
    glm::vec2 m_bounceVelocity = glm::vec2(0.3f, 1.0f);
    glm::vec2 m_stuckPosition = glm::vec2(0.0f);
    float m_radius = 0.0;
    int m_damage = 1;
    bool m_stuck = false;

   public:
    Ball(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);
    Ball(ContextPtr context, Texture2DPtr texture);

    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    bool isStuck() const noexcept;
    void setStuck(bool state) noexcept;

    float getRadius() const noexcept;
    void setRadius(float radius) noexcept;

    int getDamage() const noexcept;
    void setDamage(int damage) noexcept;

    glm::vec2 getBounceVelocity() const noexcept;
    void setBounceVelocity(glm::vec2 velocity) noexcept;
};

using BallPtr = observer_ptr<Ball>;
