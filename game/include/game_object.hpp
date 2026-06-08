#pragma once

#include <glm/glm.hpp>

#include "aabb.hpp"
#include "collision_type.hpp"
#include "object_2d.hpp"
#include "sprite_object.hpp"
#include "texture_2d.hpp"

enum class GameObjectType {
    None,
    Player,
    Ball,
    Brick,
    PowerUp
};

class GameObject;
using GameObjectPtr = observer_ptr<GameObject>;

class GameObject : public SpriteObject {
   public:
    GameObject(ContextPtr context, Texture2DPtr texture);
    GameObject(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size);

    virtual void update(float dt) = 0;
    virtual void fixedUpdate(float dt) = 0;

    virtual Collision checkCollision(GameObject &gameObject);
    virtual GameObjectType getType() const noexcept;

    virtual bool isDestroyable() const noexcept;
    virtual void setDestructibility(bool flag) noexcept;

    virtual float getSpeed() const noexcept;
    virtual void setSpeed(float speed) noexcept;

    virtual void setSize(glm::vec2 size) noexcept override;

    virtual void setColliding(bool flag) noexcept;
    virtual bool isColliding() const noexcept;

    virtual AABB getAABB() const noexcept;
    virtual void setAABB(AABB aabb) noexcept;

    virtual void resetPosition(glm::vec2 position) noexcept;
    virtual glm::vec2 getPreviousPosition() const noexcept;

    virtual void setPosition(glm::vec2 position) noexcept override;
    virtual void setLocalPosition(glm::vec2 position) noexcept override;

   protected:
    AABB m_aabb;

    glm::vec2 m_previousPosition = glm::vec3(0.0f);

    float m_accelerationAttenuation = 0.1f;
    float m_speed = 0.0f;

    GameObjectType m_type = GameObjectType::None;

    bool m_isDestroyable = false;
    bool m_isColliding = true;
};
