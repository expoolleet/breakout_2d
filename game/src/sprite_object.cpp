#include "sprite_object.hpp"

SpriteObject::SpriteObject(ContextPtr context, Texture2DPtr texture) : Object2D(context), m_texture(texture) {
    assert(m_texture && "Texture is null!");
}

SpriteObject::SpriteObject(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size)
    : Object2D(context, position, size), m_shader(nullptr) {
    assert(texture != nullptr && "Texture address is null");
    setPosition(position);
    m_texture = texture;
}

ShaderPtr SpriteObject::getShader() const noexcept {
    return m_shader;
}

void SpriteObject::setShader(ShaderPtr shader) {
    assert(shader != nullptr && "Shader pointer is null!");
    m_shader = shader;
}

void SpriteObject::setColor(glm::vec4 color) noexcept {
    m_color = color;
}

glm::vec4 SpriteObject::getColor() const noexcept {
    return m_color;
}

Texture2DPtr SpriteObject::getTexture() const noexcept {
    return m_texture;
}

void SpriteObject::setTexture(Texture2DPtr texture) {
    assert(texture && "Texture is nullptr!");
    m_texture = texture;
}
