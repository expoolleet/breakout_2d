#pragma once

#include "engine_context.hpp"
#include "scene_node.hpp"
#include "shader.hpp"
#include "texture_2d.hpp"

class Object2D;
using Object2DPtr = observer_ptr<Object2D>;

class Object2D {
   public:
    Object2D(ContextPtr context);
    Object2D(ContextPtr context, Texture2DPtr texture);
    Object2D(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);

    virtual ~Object2D() noexcept;
    Object2D(const Object2D &) = default;
    Object2D &operator=(const Object2D &) = default;
    Object2D(Object2D &&) noexcept = default;
    Object2D &operator=(Object2D &&) = default;

    virtual ShaderPtr getShader() const noexcept;
    virtual void setShader(ShaderPtr shader);

    virtual Texture2DPtr getTexture() const noexcept;
    virtual void setTexture(Texture2DPtr texture);

    virtual glm::vec2 getPosition() const noexcept;
    virtual glm::vec2 getLocalPosition() const noexcept;
    virtual void setPosition(glm::vec2 position) noexcept;
    virtual void setLocalPosition(glm::vec2 position) noexcept;

    virtual glm::vec2 getSize() const noexcept;
    virtual void setSize(glm::vec2 size) noexcept;

    virtual glm::vec4 getColor() const noexcept;
    virtual void setColor(glm::vec4 color) noexcept;

    virtual bool isHidden() const noexcept;
    virtual void hide(bool state) noexcept;

    virtual bool isAlive() const noexcept;

    virtual void destroy() noexcept;

    virtual void reset() noexcept;

    void setParent(Object2DPtr parent);
    void addChild(Object2DPtr child);
    void clearChildren() noexcept;
    Object2DPtr getParent() const noexcept;

   protected:
    ContextPtr m_context;
    ShaderPtr m_shader;
    Texture2DPtr m_texture;

    SceneNode<Object2D> m_node;

    glm::vec2 m_position = glm::vec3(0.0f);
    glm::vec2 m_localPosition = glm::vec3(0.0f);
    glm::vec2 m_size = glm::vec3(0.0f);
    glm::vec4 m_color = glm::vec4(1.0f);

    bool m_isHidden = false;
    bool m_isAlive = true;

    void _addChildUnsafe(Object2DPtr child);
    void _removeChildUnsafe(Object2DPtr child);
};
