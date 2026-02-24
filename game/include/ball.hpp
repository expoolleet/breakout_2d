#pragma once

#include "collision_type.hpp"
#include "game_object.hpp"

#include <glm/glm.hpp>

#define BALL_DEFAULT_SPEED 800.0f
#define BALL_DEFAULT_SIZE glm::vec2(31.0f)

class Texture2D; // fwd
class Player;

class Ball : public GameObject {
  private:
    glm::vec2 m_bounceVelocity = glm::vec2(0.3f, 1.0f);
    const Player *m_player = nullptr;
    float m_radius = 0.0;
    unsigned int m_damage = 1;
    bool m_stuck = true;

  public:
    Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size, const Player &player);
    Ball(const Texture2D &texture);
    Ball();

    void update(float dt) override;
    void fixedUpdate(float dt) override;
    Collision checkCollision(GameObject &gameObject) override;

    void assignPlayer(const Player &player);

    void reset() override;

    void move(float dt);

    bool isStuck();
    void setStuck(bool state);

    void setRadius(float radius);
    float getRadius();

    void setDamage(unsigned int damage);
    unsigned int getDamage();

    glm::vec2 getBounceVelocity();
    void setBounceVelocity(glm::vec2 velocity);
};
