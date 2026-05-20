#include "game_object.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "collision_detection.hpp"
#include "collision_type.hpp"

GameObject::GameObject(const Texture2D &texture, glm::vec2 position, glm::vec2 size) : Object2D(texture, position, size) {}

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
