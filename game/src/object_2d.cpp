#include "object_2d.hpp"

#include <cassert>
#include <glm/glm.hpp>

Object2D::Object2D(ContextPtr context) : m_context(context) {}

Object2D::Object2D(ContextPtr context, glm::vec2 position, glm::vec2 size) : m_context(context), m_size(size) {
    setPosition(position);
}

Object2D::~Object2D() noexcept {
    for (auto &child : getChildren()) {
        child->destroy();
    }
}

bool Object2D::isHidden() const noexcept {
    return m_isHidden;
}

void Object2D::hide(bool hidden) noexcept {
    m_isHidden = hidden;
}

glm::vec2 Object2D::getSize() const noexcept {
    return m_size;
}

void Object2D::setSize(glm::vec2 size) noexcept {
    m_size = size;
}

bool Object2D::isAlive() const noexcept {
    return m_isAlive;
}

void Object2D::destroy() noexcept {
    m_isHidden = true;
    m_isAlive = false;
    for (auto &child : getChildren()) {
        child->destroy();
    }
}

void Object2D::reset() noexcept {
    m_isHidden = false;
    m_isAlive = true;
    for (auto &child : getChildren()) {
        child->reset();
    }
}

glm::vec2 Object2D::getPosition() const noexcept {
    if (m_node.m_parent) {
        return m_node.m_parent->getPosition() + m_localPosition;
    }
    return m_position;
}

glm::vec2 Object2D::getLocalPosition() const noexcept {
    return m_localPosition;
}

void Object2D::setPosition(glm::vec2 position) noexcept {
    m_position = position;
    if (m_node.m_parent) {
        m_localPosition = position - m_node.m_parent->getPosition();
    } else {
        m_localPosition = position;
    }
    for (auto &child : m_node.m_children) {
        child->setPosition(child->getLocalPosition() + m_position);
    }
}

void Object2D::setLocalPosition(glm::vec2 position) noexcept {
    if (m_node.m_parent) {
        setPosition(m_node.m_parent->getPosition() + position);
    } else {
        m_localPosition = position;
    }
}

glm::vec2 Object2D::getVelocity() const noexcept {
    return m_velocity;
}

void Object2D::setVelocity(glm::vec2 velocity) noexcept {
    m_velocity = velocity;
}
