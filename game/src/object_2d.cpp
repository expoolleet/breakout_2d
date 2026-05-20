#include "object_2d.hpp"

#include <cassert>
#include <glm/glm.hpp>

Object2D::Object2D(Texture2DRef texture, glm::vec2 position, glm::vec2 size) : m_position(position), m_size(size), m_texture(&texture) {
    assert(Texture != nullptr && "Texture address is null");
}

Object2D::Object2D(Texture2DRef texture) : m_texture(&texture) {
    assert(Texture != nullptr && "Texture address is null");
}

ShaderRef Object2D::getShader() const noexcept {
    return *m_shader;
}

void Object2D::setShader(ShaderRef shader) {
    m_shader = &shader;
}

Texture2DRef Object2D::getTexture() const noexcept {
    return *m_texture;
}

void Object2D::setTexture(Texture2DRef texture) {
    m_texture = &texture;
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
