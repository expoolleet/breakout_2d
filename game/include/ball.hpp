#pragma once

#include <glm/glm.hpp>

#include "collision_type.hpp"
#include "game_object.hpp"

#define BALL_DEFAULT_SPEED 20.0f
#define BALL_DEFAULT_SIZE glm::vec2(0.65f)

class Texture2D;  // fwd
class Player;

class Ball : public GameObject {
   private:
    glm::vec2 m_bounceVelocity = glm::vec2(0.3f, 1.0f);
    glm::vec2 m_stuckPosition = glm::vec2(0.0f);
    const Player *m_player = nullptr;
    float m_radius = 0.0;
    unsigned int m_damage = 1;
    bool m_stuck = false;

   public:
    Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size, const Player &player);
    Ball(const Texture2D &texture);
    Ball();

    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    void assignPlayer(const Player &player);

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
};
