#include "brick.hpp"

#include <algorithm>
#include <glm/glm.hpp>

#include "brick_data.hpp"
#include "brick_type.hpp"
#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "game_object.hpp"
#include "tween.hpp"

Brick::Brick(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size, BrickType type)
    : GameObject(context, texture, position, size), m_brickType(type) {
    BrickData data = getBrickData(type);
    m_isDestroyable = data.isDestroyable;
    m_color = data.color;
    m_hardnessPoints = data.maxHardnessPoints;
    m_maxHardnessPoints = m_hardnessPoints;
    m_getGameObjectType = GameObjectType::Brick;
}

void Brick::update(float dt) {}

void Brick::fixedUpdate(float dt) {}

Collision Brick::checkCollision(GameObject &gameObject) {
    return cd::NoneCollision;
}

void Brick::setHardenessPoints(int hp) noexcept {
    assert(hp >= 1 && "Hardeness points of brick needs to be above 1");
    m_hardnessPoints = hp;
}

int Brick::getCurrentHardnessPoints() const noexcept {
    return m_hardnessPoints;
}

int Brick::getMaxHardnessPoints() const noexcept {
    return m_maxHardnessPoints;
}

void Brick::damage(int damage) {
    assert(damage >= 0 && "Damage needs to be above 0");
    m_hardnessPoints = std::max(0, m_hardnessPoints - damage);
    if (m_hardnessPoints == 0) {
        TweenPtr tween = Tween::createTween();
        tween->tweenProperty([&](TweenValue value) { setColor(std::get<glm::vec4>(value)); }, m_color,
                             glm::vec4(m_color.r, m_color.g, m_color.b, 0.0f), 0.1f);
        tween->tweenCallback([&]() { destroy(); });
    }
}

void Brick::heal(int heal) {
    assert(heal >= 0 && "Heal needs to be above 0");
    m_hardnessPoints = std::min(m_maxHardnessPoints, m_hardnessPoints + heal);
}

void Brick::reset() noexcept {
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
