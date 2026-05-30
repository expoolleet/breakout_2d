#include "object_2d.hpp"

#include <cassert>
#include <glm/glm.hpp>

Object2D::Object2D(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size)
    : m_context(context), m_shader(nullptr), m_texture(texture), m_position(position), m_size(size) {
    assert(texture != nullptr && "Texture address is null");
}

Object2D::Object2D(ContextPtr context, Texture2DPtr texture) : m_context(context), m_shader(nullptr), m_texture(texture) {
    assert(texture != nullptr && "Texture address is null");
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

glm::vec2 Object2D::getPosition() const {
    return m_position;
}

glm::vec2 Object2D::getSize() const noexcept {
    return m_size;
}

void Object2D::setPosition(glm::vec2 position) {
    m_position = position;
}

void Object2D::setSize(glm::vec2 size) {
    m_size = size;
}

void Object2D::setColor(glm::vec4 color) {
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

void Object2D::reset() {
    m_isHidden = false;
    m_isAlive = true;
}
