#include "player.hpp"

#include <glm/glm.hpp>

#include "game_core.hpp"
#include "game_object.hpp"
#include "input.hpp"
#include "texture_2d.hpp"

Player::Player(const Texture2D &texture, glm::vec2 position, glm::vec2 size) : GameObject(texture, position, size) {
    m_type = GameObjectType::Player;
}

void Player::input(const Keys &keys) {
    if (keys.isPressed(KEY_A)) {
        m_velocity.x = -1.0f;
    } else if (keys.isPressed(KEY_D)) {
        m_velocity.x = 1.0f;
    } else {
        m_velocity.x = 0.0f;
    }
}

void Player::update(float dt) {}

void Player::fixedUpdate(float dt) {
    glm::vec2 playerPosition = getPosition();
    playerPosition += m_velocity * m_speed * dt;

    glm::vec4 worldAABB = core::getWorldAABB();
    float offset = 0.5f;
    float leftSide = worldAABB.x + offset;
    float rightSide = worldAABB.z - getSize().x - offset;
    if (playerPosition.x <= leftSide) {
        playerPosition.x = leftSide;
    }
    if (playerPosition.x >= rightSide) {
        playerPosition.x = rightSide;
    }
    setPosition(playerPosition);
}

void Player::setStrength(float strength) {
    m_strength = strength;
}

float Player::getStrength() const noexcept {
    return m_strength;
}

bool Player::isSticky() const noexcept {
    return m_sticky;
}

void Player::setStickness(bool state) noexcept {
    m_sticky = state;
}
