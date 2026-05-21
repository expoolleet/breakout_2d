#include "game_object.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "collision_detection.hpp"
#include "collision_type.hpp"

GameObject::GameObject(const Texture2D &texture, glm::vec2 position, glm::vec2 size) : Object2D(texture, position, size) {
    m_aabb.halfSize = glm::vec2(getSize().x / 2, getSize().y / 2);
    m_aabb.center = glm::vec2(getPosition().x + m_aabb.halfSize.x, getPosition().y + m_aabb.halfSize.y);
}

GameObject::GameObject(const Texture2D &texture) : Object2D(texture) {}

Collision GameObject::checkCollision(GameObject &gameObject) {
    return cd::NoneCollision;
}

GameObjectType GameObject::getObjectType() const noexcept {
    return m_type;
}

bool GameObject::isDestroyable() const noexcept {
    return m_isDestroyable;
}

bool GameObject::isDead() const noexcept {
    return m_isDead;
}

void GameObject::setDestructibility(bool destroyable) noexcept {
    m_isDestroyable = destroyable;
}

void GameObject::destroy() noexcept {
    m_isDead = true;
    m_isHidden = true;
}

void GameObject::reset() {
    m_isDead = false;
    m_isHidden = false;
}

glm::vec2 GameObject::getPreviousPosition() {
    return m_previousPosition;
}

void GameObject::setPosition(glm::vec2 position) {
    m_previousPosition = m_position;
    m_position = position;
    m_aabb.center = glm::vec2(m_position.x + m_aabb.halfSize.x, m_position.y + m_aabb.halfSize.y);
}

void GameObject::resetPosition(glm::vec2 position) {
    m_previousPosition = position;
    m_position = position;
}

glm::vec2 GameObject::getVelocity() const noexcept {
    return m_velocity;
}

void GameObject::setVelocity(glm::vec2 velocity) {
    m_velocity = velocity;
}

float GameObject::getSpeed() const noexcept {
    return m_speed;
}

void GameObject::setSpeed(float speed) {
    m_speed = speed;
}

void GameObject::setColliding(bool colliding) {
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
