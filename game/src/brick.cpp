#include "brick.hpp"

#include <algorithm>
#include <glm/glm.hpp>

#include "ball.hpp"
#include "brick_data.hpp"
#include "brick_type.hpp"
#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "game_object.hpp"
#include "powerup_data.hpp"

Brick::Brick(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size, BrickType type)
    : GameObject(context, texture, position, size), m_brickType(type) {
    BrickData data = getBrickData(type);
    setDestructibility(data.isDestroyable);
    setColor(data.color);
    m_hardnessPoints = data.maxHardnessPoints;
    m_maxHardnessPoints = m_hardnessPoints;
    m_type = GameObjectType::Brick;
}

void Brick::update(float dt) {}

void Brick::fixedUpdate(float dt) {}

Collision Brick::checkCollision(GameObject &gameObject) {
    if (Ball *ball = dynamic_cast<Ball *>(&gameObject)) {
        Collision collision = cd::checkCollision(*ball, *this);
        if (std::get<0>(collision)) {
            if (isDestroyable()) {
                doDamage(ball->getDamage());
            }
            return collision;
        }
    }
    return cd::NoneCollision;
}

int Brick::getCurrentHardnessPoints() const noexcept {
    return m_hardnessPoints;
}

int Brick::getMaxHardnessPoints() const noexcept {
    return m_maxHardnessPoints;
}

void Brick::doDamage(unsigned int damage) {
    m_hardnessPoints = std::max(0, m_hardnessPoints - static_cast<int>(damage));
    if (m_hardnessPoints == 0) {
        destroy();
    }
}

void Brick::doHeal(unsigned int heal) {
    m_hardnessPoints = std::min(m_maxHardnessPoints, m_hardnessPoints + static_cast<int>(heal));
}

void Brick::reset() {
    GameObject::reset();
    m_hardnessPoints = m_maxHardnessPoints;
}

BrickType Brick::getBrickType() const noexcept {
    return m_brickType;
}

PowerUpType Brick::getPowerUpType() const noexcept {
    return m_powerUp;
}

void Brick::setPowerUpType(PowerUpType type) {
    m_powerUp = type;
}
