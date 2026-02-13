#include "game_object.hpp"

#include "collision_detection.hpp"
#include "collision_type.hpp"

#include <cassert>
#include <glm/glm.hpp>

GameObject::GameObject(const Texture2D &texture, glm::vec2 position, glm::vec2 size)
    : m_position(position), m_size(size), Texture(&texture) {
    assert(Texture != nullptr && "Texture address is null");
}

GameObject::GameObject(const Texture2D &texture) : Texture(&texture) {
    assert(Texture != nullptr && "Texture address is null");
}

Collision GameObject::checkCollision(GameObject &gameObject) {
    return _cd::NoneCollision;
}

bool GameObject::isDestroyable() const {
    return m_isDestroyable;
}

bool GameObject::isDead() const {
    return m_isDead;
}

void GameObject::setDestructibility(bool flag) {
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

bool GameObject::IsHidden() const {
    return m_isHidden;
}

void GameObject::Hide(bool state) {
    m_isHidden = state;
}

glm::vec2 GameObject::getPosition() const {
    return m_position;
}

glm::vec2 GameObject::getPreviousPosition() {
    return m_previousPosition;
}

glm::vec2 GameObject::getSize() const {
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

glm::vec2 GameObject::getVelocity() const {
    return m_velocity;
}

void GameObject::setVelocity(glm::vec2 velocity) {
    m_velocity = velocity;
}

float GameObject::getSpeed() const {
    return m_speed;
}

void GameObject::setSpeed(float speed) {
    m_speed = speed;
}
