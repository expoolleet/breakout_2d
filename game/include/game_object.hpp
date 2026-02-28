#pragma once

#include <glm/glm.hpp>

#include "collision_type.hpp"
#include "texture_2d.hpp"

enum GameObjectType {
    GameObject_None,
    GameObject_Player,
    GameObject_Ball,
    GameObject_Brick,
    GameObject_PowerUp
};

class GameObject {
   protected:
    bool m_isDestroyable = false;
    bool m_isDead = false;
    bool m_isHidden = false;
    bool m_isColliding = true;
    glm::vec2 m_position = glm::vec3(0.0f);
    glm::vec2 m_previousPosition = glm::vec3(0.0f);
    glm::vec2 m_size = glm::vec3(0.0f);
    glm::vec2 m_velocity = glm::vec2(0.0f);
    glm::vec2 m_acceleration = glm::vec2(0.0f);
    glm::vec4 m_color = glm::vec4(1.0f);
    float m_accelerationAttenuation = 0.1f;
    float m_speed = 0.0f;
    GameObjectType m_type = GameObjectType::GameObject_None;

   public:
    const Texture2D *Texture = nullptr;

    GameObject(const Texture2D &texture, glm::vec2 position, glm::vec2 size);
    GameObject(const Texture2D &texture);

    virtual ~GameObject() = default;

    virtual void update(float dt) = 0;
    virtual void fixedUpdate(float dt) = 0;
    virtual Collision checkCollision(GameObject &gameObject);
    virtual GameObjectType getObjectType() const;

    virtual bool isDestroyable() const;
    virtual bool isDead() const;
    virtual bool isHidden() const;
    virtual void setDestructibility(bool flag);
    virtual void destroy();
    virtual void hide(bool state);
    virtual void reset();

    virtual glm::vec2 getPosition() const;
    virtual glm::vec2 getPreviousPosition();
    virtual void setPosition(glm::vec2 position);
    virtual void resetPosition(glm::vec2 position);

    virtual glm::vec2 getSize() const;
    virtual void setSize(glm::vec2 size);

    virtual glm::vec2 getVelocity() const;
    virtual void setVelocity(glm::vec2 velocity);

    virtual float getSpeed() const;
    virtual void setSpeed(float speed);

    virtual void setColor(glm::vec4 color);
    virtual glm::vec4 getColor() const;

    virtual void setColliding(bool flag);
    virtual bool isColliding();
};
