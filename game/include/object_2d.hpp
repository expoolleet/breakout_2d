#pragma once

#include "engine_context.hpp"
#include "interfaces/scene_node_interface.hpp"

// Curiously Recurring Template pattern
class Object2D : public SceneNodeInterface<Object2D> {
   public:
    Object2D() = default;
    Object2D(ContextPtr context);
    Object2D(ContextPtr context, glm::vec2 position, glm::vec2 size);

    virtual ~Object2D() noexcept override;
    Object2D(const Object2D &) = default;
    Object2D &operator=(const Object2D &) = default;
    Object2D(Object2D &&) noexcept = default;
    Object2D &operator=(Object2D &&) = default;

    virtual glm::vec2 getSize() const noexcept;
    virtual void setSize(glm::vec2 size) noexcept;

    virtual bool isHidden() const noexcept;
    virtual void hide(bool state) noexcept;

    virtual bool isAlive() const noexcept;

    virtual void destroy() noexcept;

    virtual void reset() noexcept;

    virtual glm::vec2 getPosition() const noexcept;
    virtual glm::vec2 getLocalPosition() const noexcept;
    virtual void setPosition(glm::vec2 position) noexcept;
    virtual void setLocalPosition(glm::vec2 position) noexcept;

    virtual glm::vec2 getVelocity() const noexcept;
    virtual void setVelocity(glm::vec2 velocity) noexcept;

   protected:
    ContextPtr m_context;

    glm::vec2 m_position = glm::vec3(0.0f);
    glm::vec2 m_localPosition = glm::vec3(0.0f);

    glm::vec2 m_velocity = glm::vec2(0.0f);

    glm::vec2 m_size = glm::vec3(0.0f);

    bool m_isHidden = false;
    bool m_isAlive = true;
};

using Object2DPtr = observer_ptr<Object2D>;
