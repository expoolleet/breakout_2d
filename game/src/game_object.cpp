#include "game_object.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "sprite_object.hpp"

GameObject::GameObject(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size)
    : SpriteObject(context, texture, position, size) {}

GameObject::GameObject(ContextPtr context, Texture2DPtr texture) : SpriteObject(context, texture) {}

Collision GameObject::checkCollision(GameObject &gameObject) {
    return cd::NoneCollision;
}

GameObjectType GameObject::getGameObjectType() const noexcept {
    return m_getGameObjectType;
}

bool GameObject::isDestroyable() const noexcept {
    return m_isDestroyable;
}

void GameObject::setDestructibility(bool destroyable) noexcept {
    m_isDestroyable = destroyable;
}

void GameObject::setPosition(glm::vec2 position) noexcept {
    m_previousPosition = m_position;
    Object2D::setPosition(position);
}

glm::vec2 GameObject::getPreviousPosition() const noexcept {
    return m_previousPosition;
}

void GameObject::resetPosition(glm::vec2 position) noexcept {
    m_previousPosition = position;
    m_aabb.center = glm::vec2(position.x + m_aabb.halfSize.x, position.y + m_aabb.halfSize.y);
    Object2D::setPosition(position);
}

float GameObject::getSpeed() const noexcept {
    return m_speed;
}

void GameObject::setSpeed(float speed) noexcept {
    m_speed = speed;
}

void GameObject::setColliding(bool colliding) noexcept {
    m_isColliding = colliding;
}

bool GameObject::isColliding() const noexcept {
    return m_isColliding;
}
