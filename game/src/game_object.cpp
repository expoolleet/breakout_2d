#include "game_object.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "sprite_object.hpp"

GameObject::GameObject(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size)
    : SpriteObject(context, texture, position, size) {
    m_aabb.halfSize = glm::vec2(getSize().x / 2, getSize().y / 2);
    m_aabb.center = glm::vec2(getPosition().x + m_aabb.halfSize.x, getPosition().y + m_aabb.halfSize.y);
}

GameObject::GameObject(ContextPtr context, Texture2DPtr texture) : SpriteObject(context, texture) {}

Collision GameObject::checkCollision(GameObject &gameObject) {
    return cd::NoneCollision;
}

GameObjectType GameObject::getType() const noexcept {
    return m_type;
}

bool GameObject::isDestroyable() const noexcept {
    return m_isDestroyable;
}

void GameObject::setDestructibility(bool destroyable) noexcept {
    m_isDestroyable = destroyable;
}

void GameObject::setSize(glm::vec2 size) noexcept {
    m_size = size;
    m_aabb.halfSize = glm::vec2(m_size.x / 2, m_size.y / 2);
}

void GameObject::setPosition(glm::vec2 position) noexcept {
    m_previousPosition = m_position;
    m_aabb.center = position + m_aabb.halfSize;
    Object2D::setPosition(position);
}

void GameObject::setLocalPosition(glm::vec2 position) noexcept {
    if (m_node.m_parent) {
        m_aabb.center = m_node.m_parent->getPosition() + position + m_aabb.halfSize;
    } else {
        m_aabb.center = position + m_aabb.halfSize;
    }
    Object2D::setLocalPosition(position);
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

AABB GameObject::getAABB() const noexcept {
    return m_aabb;
}

void GameObject::setAABB(AABB aabb) noexcept {
    m_aabb = std::move(aabb);
}
