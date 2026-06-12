#include "powerup.hpp"

#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "engine_context.hpp"
#include "event_type.hpp"
#include "fast_random.hpp"
#include "game_core.hpp"
#include "powerup_type.hpp"

PowerUp::PowerUp(ContextPtr context, Texture2DPtr texture, PowerUpType type, glm::vec4 color, float duration, glm::vec2 position,
                 glm::vec2 size)
    : GameObject(context, texture, position, size), m_powerUpType(type), m_duration(duration), m_currentDuration(duration) {
    m_getGameObjectType = GameObjectType::PowerUp;
    setVelocity(glm::vec2(0.0f, -1.0f));
    setSpeed(fastrand::frandomFloatInRange(5.0f, 8.0f));
    setColor(color);
}

void PowerUp::update(float dt) {}

void PowerUp::fixedUpdate(float dt) {
    if (!isAlive()) {
        return;
    } else if (isActivated()) {
        m_currentDuration -= dt;

        if (isFinished()) {
            m_context->getEventDispatcher().emit(PowerUpFinished{this});
            destroy();
        }
    } else if (m_position.y <= core::getWorldAABB().y - m_size.y) {
        destroy();
    } else {
        m_previousPosition = m_position;
        m_position += m_velocity * m_speed * dt;
    }
}

bool PowerUp::isActivated() {
    return m_activated;
}

bool PowerUp::isFinished() {
    return m_currentDuration <= 0.0f;
}

void PowerUp::activate() {
    m_activated = true;
}

void PowerUp::reset() noexcept {
    m_currentDuration = m_duration;
}

PowerUpType PowerUp::getPowerUpType() const {
    return m_powerUpType;
}

Collision PowerUp::checkCollision(GameObject &gameObject) {
    Collision collision = cd::checkCollision(*this, gameObject);
    if (std::get<0>(collision) && !m_activated && gameObject.getGameObjectType() == GameObjectType::Player) {
        hide(true);
        activate();
        m_context->getEventDispatcher().emit(PowerUpActivated{this});
    }
    return collision;
}
