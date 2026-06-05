#include "object_2d.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "logging.hpp"

Object2D::Object2D(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size)
    : m_context(context), m_shader(nullptr), m_texture(texture), m_position(position), m_size(size) {
    assert(texture != nullptr && "Texture address is null");
}

Object2D::Object2D(ContextPtr context, Texture2DPtr texture) : m_context(context), m_shader(nullptr), m_texture(texture) {
    assert(texture != nullptr && "Texture address is null");
}

Object2D::~Object2D() noexcept {
    for (auto &child : m_children) {
        child->destroy();
    }
}

ShaderPtr Object2D::getShader() const noexcept {
    return m_shader;
}

void Object2D::setShader(ShaderPtr shader) {
    assert(shader != nullptr && "Shader pointer is null!");
    m_shader = shader;
}

Texture2DPtr Object2D::getTexture() const noexcept {
    return m_texture;
}

void Object2D::setTexture(Texture2DPtr texture) {
    assert(texture != nullptr && "Texture pointer is null!");
    m_texture = texture;
}

bool Object2D::isHidden() const noexcept {
    return m_isHidden;
}

void Object2D::hide(bool hidden) noexcept {
    m_isHidden = hidden;
}

glm::vec2 Object2D::getPosition() const noexcept {
    if (m_parent) {
        return m_parent->getPosition() + m_localPosition;
    }
    return m_position;
}

glm::vec2 Object2D::getLocalPosition() const noexcept {
    return m_localPosition;
}

glm::vec2 Object2D::getSize() const noexcept {
    return m_size;
}

void Object2D::setPosition(glm::vec2 position) noexcept {
    m_position = position;
    for (auto &child : m_children) {
        child->setPosition(child->getLocalPosition() + m_position);
    }
}

void Object2D::setLocalPosition(glm::vec2 position) noexcept {
    m_localPosition = position;
    if (m_parent) {
        setPosition(m_parent->getPosition() + m_localPosition);
    }
}

void Object2D::setSize(glm::vec2 size) noexcept {
    m_size = size;
}

void Object2D::setColor(glm::vec4 color) noexcept {
    m_color = color;
}

glm::vec4 Object2D::getColor() const noexcept {
    return m_color;
}

bool Object2D::isAlive() const noexcept {
    return m_isAlive;
}

void Object2D::destroy() noexcept {
    m_isHidden = true;
    m_isAlive = false;
}

void Object2D::reset() noexcept {
    m_isHidden = false;
    m_isAlive = true;
}

void Object2D::setParent(Object2DPtr parent) {
    if (this == parent.get()) {
        logging::Error("(Object2D) Cannot set parent to self");
        return;
    }
    Object2DPtr oldParent = m_parent;
    m_parent = parent;
    if (oldParent) {
        oldParent->removeChild(this);
    }
    if (m_parent) {
        m_parent->addChild(this);
    }
}

Object2DPtr Object2D::getParent() const noexcept {
    return m_parent;
}

void Object2D::addChild(Object2DPtr child) {
    if (this == child.get()) {
        logging::Error("(Object2D) Cannot set child to self");
        return;
    }
    if (!child->getParent()) child->setParent(this);
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        logging::Error("(Object2D) Cannot set child to object twice");
        return;
    }
    m_children.push_back(child);
}

void Object2D::removeChild(Object2DPtr child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it == m_children.end()) {
        logging::Error("(Object2D) Could not find a child to delete from parent");
    }
    m_children.erase(it);
    if (child->getParent()) child->setParent(nullptr);
}

void Object2D::clearChildren() noexcept {
    if (m_children.empty()) return;
    for (auto &child : m_children) {
        if (child->getParent()) child->setParent(nullptr);
    }
    m_children.clear();
}
