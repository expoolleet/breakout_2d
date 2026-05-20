#pragma once

#include "texture_2d.hpp"

class Object2D {
   protected:
    Texture2DView m_texture = nullptr;
    glm::vec2 m_position = glm::vec3(0.0f);
    glm::vec2 m_size = glm::vec3(0.0f);
    glm::vec4 m_color = glm::vec4(1.0f);
    bool m_isHidden = false;

   public:
    Object2D(Texture2DRef texture);
    Object2D(Texture2DRef texture, glm::vec2 position, glm::vec2 size);

    virtual ~Object2D() = default;
    Object2D(const Object2D &) noexcept = default;
    Object2D &operator=(const Object2D &) noexcept = default;

    virtual Texture2DRef getTexture() const noexcept;
    virtual void setTexture(Texture2DRef texture);

    virtual glm::vec2 getPosition() const;
    virtual void setPosition(glm::vec2 position);

    virtual glm::vec2 getSize() const noexcept;
    virtual void setSize(glm::vec2 size);

    virtual glm::vec4 getColor() const noexcept;
    virtual void setColor(glm::vec4 color);

    virtual bool isHidden() const noexcept;
    virtual void hide(bool state) noexcept;
};
