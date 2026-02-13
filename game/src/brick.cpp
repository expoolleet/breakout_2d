#include "brick.hpp"

#include "ball.hpp"
#include "brick_data.hpp"
#include "brick_type.hpp"
#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "game_object.hpp"

#include <algorithm>
#include <glm/glm.hpp>

Brick::Brick(const Texture2D &texture, glm::vec2 position, glm::vec2 size, BrickType type)
    : GameObject(texture, position, size), m_type(type) {
    BrickData data = getBrickData(type);
    m_isDestroyable = data.isDestroyable;
    m_hardnessPoints = data.maxHardnessPoints;
    m_maxHardnessPoints = data.maxHardnessPoints;
    Color = data.color;
}

void Brick::update(float dt) {}

void Brick::fixedUpdate(float dt) {}

Collision Brick::checkCollision(GameObject &gameObject) {
    if (Ball *ball = dynamic_cast<Ball *>(&gameObject)) {
        Collision collision = _cd::checkCollision(*ball, *this);
        if (std::get<0>(collision)) {
            if (isDestroyable()) {
                doDamage(ball->getDamage());
            }
            return collision;
        }
    }
    return _cd::NoneCollision;
}

int Brick::getCurrentHardnessPoints() const {
    return m_hardnessPoints;
}

int Brick::getMaxHardnessPoints() const {
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

BrickType Brick::getType() const {
    return m_type;
}
