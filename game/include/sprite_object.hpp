#pragma once

#include "object_2d.hpp"
#include "shader.hpp"

class SpriteObject : public Object2D {
   public:
    SpriteObject() = default;
    SpriteObject(ContextPtr context, Texture2DPtr texture);
    SpriteObject(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);

    Texture2DPtr getTexture() const noexcept;
    void setTexture(Texture2DPtr texture);

    virtual ShaderPtr getShader() const noexcept;
    virtual void setShader(ShaderPtr shader);

    virtual glm::vec4 getColor() const noexcept;
    virtual void setColor(glm::vec4 color) noexcept;

   protected:
    ShaderPtr m_shader;
    Texture2DPtr m_texture;

    glm::vec4 m_color = glm::vec4(1.0f);
};
