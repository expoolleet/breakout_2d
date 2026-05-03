#include "game_object.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "collision_detection.hpp"
#include "collision_type.hpp"

GameObject::GameObject(const Texture2D &texture, glm::vec2 position, glm::vec2 size)
    : m_position(position), m_size(size), Texture(&texture) {
    assert(Texture != nullptr && "Texture address is null");
}

GameObject::GameObject(const Texture2D &texture) : Texture(&texture) {
    assert(Texture != nullptr && "Texture address is null");
}

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

void GameObject::setDestructibility(bool flag) noexcept {
    m_isDestroyable = flag;
}

void GameObject::destroy() {
    m_isDead = true;
    m_isHidden = true;
}

void GameObject::reset() {
    m_isDead = false;
    m_isHidden = false;
}

bool GameObject::isHidden() const noexcept {
    return m_isHidden;
}

void GameObject::hide(bool state) noexcept {
    m_isHidden = state;
}

glm::vec2 GameObject::getPosition() const {
    return m_position;
}

glm::vec2 GameObject::getPreviousPosition() {
    return m_previousPosition;
}

glm::vec2 GameObject::getSize() const noexcept {
    return m_size;
}

void GameObject::setPosition(glm::vec2 position) {
    m_previousPosition = m_position;
    m_position = position;
}

void GameObject::resetPosition(glm::vec2 position) {
    m_previousPosition = position;
    m_position = position;
}

void GameObject::setSize(glm::vec2 size) {
    m_size = size;
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

void GameObject::setColor(glm::vec4 color) {
    m_color = color;
}

glm::vec4 GameObject::getColor() const noexcept {
    return m_color;
}

void GameObject::setColliding(bool flag) {
    m_isColliding = flag;
}

bool GameObject::isColliding() const noexcept {
    return m_isColliding;
}
