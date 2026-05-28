#pragma once

#include <glm/glm.hpp>

#include "aabb.hpp"
#include "collision_type.hpp"
#include "object_2d.hpp"
#include "texture_2d.hpp"

enum class GameObjectType {
    None,
    Player,
    Ball,
    Brick,
    PowerUp
};

class GameObject : public Object2D {
   protected:
    AABB m_aabb;

    glm::vec2 m_previousPosition = glm::vec3(0.0f);
    glm::vec2 m_velocity = glm::vec2(0.0f);
    glm::vec2 m_acceleration = glm::vec2(0.0f);

    float m_accelerationAttenuation = 0.1f;
    float m_speed = 0.0f;

    GameObjectType m_type = GameObjectType::None;

    bool m_isDestroyable = false;
    bool m_isColliding = true;

   public:
    GameObject(ContextPtr context, Texture2DPtr texture);
    GameObject(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);

    virtual void update(float dt) = 0;
    virtual void fixedUpdate(float dt) = 0;

    virtual Collision checkCollision(GameObject &gameObject);
    virtual GameObjectType getObjectType() const noexcept;

    virtual bool isDestroyable() const noexcept;
    virtual void setDestructibility(bool flag) noexcept;

    virtual glm::vec2 getPreviousPosition();
    virtual void resetPosition(glm::vec2 position);

    virtual glm::vec2 getVelocity() const noexcept;
    virtual void setVelocity(glm::vec2 velocity);

    virtual float getSpeed() const noexcept;
    virtual void setSpeed(float speed);

    virtual void setColliding(bool flag);
    virtual bool isColliding() const noexcept;

    virtual AABB getAABB() const noexcept;
    virtual void setAABB(AABB aabb) noexcept;

    void setPosition(glm::vec2 position) override;
};
