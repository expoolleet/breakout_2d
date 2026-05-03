#include "ball.hpp"

#include <glm/glm.hpp>

#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "event_dispatcher.hpp"
#include "event_type.hpp"
#include "game_core.hpp"
#include "game_object.hpp"
#include "logging.hpp"
#include "player.hpp"
#include "texture_2d.hpp"

Ball::Ball(const Texture2D &texture, glm::vec2 position, glm::vec2 size, const Player &player)
    : GameObject(texture, position, size), m_player(&player) {
    m_type = GameObjectType::Ball;
}

Ball::Ball(const Texture2D &texture) : GameObject(texture) {
    m_type = GameObjectType::Ball;
}

void Ball::update(float dt) {}

void Ball::fixedUpdate(float dt) {
    if (isStuck()) {
        setPosition(m_player->getPosition() + m_stuckPosition);
    } else {
        setPosition(m_position + m_velocity * m_speed * dt);
    }

    glm::vec4 worldAABB = core::getWorldAABB();
    float leftSide = worldAABB.x;
    float bottomSide = worldAABB.y;
    float rightSide = worldAABB.z;
    float topSide = worldAABB.w;
    if (m_position.y <= bottomSide) {
        EventDispatcher::Get().emit(BallFliedOff{*this});
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
        EventDispatcher::Get().emit(BallHit(m_position, *this, CollisionType::Obstacle));
    }
}

Collision Ball::checkCollision(GameObject &gameObject) {
    if (isStuck()) return cd::NoneCollision;
    Collision collision = cd::checkCollision(*this, gameObject);
    if (gameObject.getObjectType() == GameObjectType::Player && std::get<0>(collision)) {
        Player *player = static_cast<Player *>(&gameObject);
        float centerBoard = m_player->getPosition().x + m_player->getSize().x / 2;
        float distance = (getPosition().x + getRadius()) - centerBoard;
        float percentage = distance / (m_player->getSize().x / 2.0f);
        glm::vec2 newVelocity = getBounceVelocity() * percentage * player->getStrength();
        newVelocity.y = std::abs(getVelocity().y);
        setVelocity(glm::normalize(newVelocity) * glm::length(getVelocity()));
    }
    return collision;
}

void Ball::assignPlayer(const Player &player) {
    m_player = &player;
}

void Ball::reset() {
    GameObject::reset();
}

bool Ball::isStuck() const noexcept {
    return m_stuck;
}

void Ball::setStuck(bool state) {
    m_stuck = state;
}

void Ball::setRadius(float radius) {
    m_radius = radius;
}

float Ball::getRadius() const noexcept {
    return m_radius;
}

void Ball::setDamage(unsigned int damage) {
    m_damage = damage;
}

unsigned int Ball::getDamage() const noexcept {
    return m_damage;
}

glm::vec2 Ball::getBounceVelocity() const noexcept {
    return m_bounceVelocity;
}

void Ball::setBounceVelocity(glm::vec2 velocity) {
    m_bounceVelocity = velocity;
}

void Ball::setStuckLocalPosition(glm::vec2 stuckPosition) {
    m_stuckPosition = stuckPosition;
}
