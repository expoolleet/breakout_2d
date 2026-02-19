#pragma once

#include "game_object.hpp"
#include "texture_2d.hpp"

#include <glm/glm.hpp>

class Player : public GameObject {
  private:
    float m_strength = 2.0f;
    bool m_sticky = false;

  public:
    Player(const Texture2D &texture, glm::vec2 position, glm::vec2 size);

    void update(float dt) override;
    void fixedUpdate(float dt) override;

    void setStrength(float strength);
    float getStrength();

    bool isSticky();
    void setStickyState(bool state);
};
