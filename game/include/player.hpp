#pragma once

#include <glm/glm.hpp>

#include "game_core.hpp"
#include "game_object.hpp"
#include "input.hpp"
#include "texture_2d.hpp"

inline constexpr glm::vec2 PLAYER_DEFAULT_SIZE = glm::vec2(5.0f, 0.7f);
inline constexpr glm::vec2 PLAYER_START_POSITION = glm::vec2(-PLAYER_DEFAULT_SIZE.x / 2.0f, -(core::getWorldHeight() / 2.0f) + 1.0f);

class Player : public GameObject {
   private:
    float m_strength = 2.0f;
    bool m_sticky = false;

   public:
    Player(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);

    void input(const Keys &keys);

    void update(float dt) override;
    void fixedUpdate(float dt) override;

    float getStrength() const noexcept;
    void setStrength(float strength);
};

using PlayerPtr = observer_ptr<Player>;
