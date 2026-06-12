#include "ball.hpp"

#include <glm/glm.hpp>

#include "brick.hpp"
#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "event_type.hpp"
#include "game_core.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "texture_2d.hpp"

Ball::Ball(ContextPtr context, Texture2DPtr texture, glm::vec2 position, glm::vec2 size) : GameObject(context, texture, position, size) {
    m_getGameObjectType = GameObjectType::Ball;
}

Ball::Ball(ContextPtr context, Texture2DPtr texture) : GameObject(context, texture) {
    m_getGameObjectType = GameObjectType::Ball;
}

void Ball::update(float dt) {}

void Ball::fixedUpdate(float dt) {
    if (!isStuck()) {
        setPosition(m_position + m_velocity * m_speed * dt);
    }

    glm::vec4 worldAABB = core::getWorldAABB();
    float leftSide = worldAABB.x;
    float bottomSide = worldAABB.y;
    float rightSide = worldAABB.z;
    float topSide = worldAABB.w;
    if (m_position.y <= bottomSide) {
        m_context->getEventDispatcher().emit(BallFliedOff{this});
        return;
    }
    if (m_position.x > leftSide && m_position.x + m_size.x < rightSide && m_position.y + m_size.y < topSide)
        return;
    else {
        if (m_position.x <= leftSide) {
            m_position.x = leftSide;
            m_velocity.x = -m_velocity.x;
        } else if (m_position.x + m_size.x >= rightSide) {
            m_position.x = rightSide - m_size.x;
            m_velocity.x = -m_velocity.x;
        } else if (m_position.y + m_size.y >= topSide) {
            m_position.y = topSide - m_size.y;
            m_velocity.y = -m_velocity.y;
        }
        m_context->getEventDispatcher().emit(BallHit{this, m_position, CollisionType::Obstacle});
    }
}

Collision Ball::checkCollision(GameObject &gameObject) {
    if (isStuck()) return cd::NoneCollision;
    Collision collision = cd::checkCollision(*this, gameObject);
    if (!std::get<0>(collision)) {
        return cd::NoneCollision;
    }

    if (gameObject.getGameObjectType() == GameObjectType::Player) {
        float centerBoard = gameObject.getPosition().x + gameObject.getSize().x / 2;
        float distance = (getPosition().x + getRadius()) - centerBoard;
        float percentage = distance / (gameObject.getSize().x / 2.0f);
        glm::vec2 newVelocity = getBounceVelocity() * percentage * static_cast<Player *>(&gameObject)->getStrength();
        newVelocity.y = std::abs(getVelocity().y);
        setVelocity(glm::normalize(newVelocity) * glm::length(getVelocity()));
    } else if (gameObject.getGameObjectType() == GameObjectType::Brick) {
        BrickPtr brick = dynamic_cast<Brick *>(&gameObject);
        if (brick->isDestroyable()) {
            brick->damage(m_damage);
        }
    }
    return collision;
}

bool Ball::isStuck() const noexcept {
    return m_stuck;
}

void Ball::setStuck(bool state) noexcept {
    m_stuck = state;
}

void Ball::setRadius(float radius) noexcept {
    m_radius = radius;
}

float Ball::getRadius() const noexcept {
    return m_radius;
}

void Ball::setDamage(int damage) noexcept {
    m_damage = damage;
}

int Ball::getDamage() const noexcept {
    return m_damage;
}

glm::vec2 Ball::getBounceVelocity() const noexcept {
    return m_bounceVelocity;
}

void Ball::setBounceVelocity(glm::vec2 velocity) noexcept {
    m_bounceVelocity = velocity;
}
