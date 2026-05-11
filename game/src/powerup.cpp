#include "powerup.hpp"

#include "collision_detection.hpp"
#include "collision_type.hpp"
#include "engine_context.hpp"
#include "event_type.hpp"
#include "fast_random.hpp"
#include "game_core.hpp"
#include "logging.hpp"
#include "powerup_type.hpp"

PowerUp::PowerUp(PowerUpType type, glm::vec4 color, float duration, const Texture2D &texture, glm::vec2 position, glm::vec2 size)
    : GameObject(texture, position, size), m_powerUpType(type), m_duration(duration) {
    m_type = GameObjectType::PowerUp;
    setVelocity(glm::vec2(0.0, -1.0f));
    setSpeed(fastrand::frandomFloatInRange(5.0f, 8.0f));
    setColor(color);
}

void PowerUp::update(float dt) {}

void PowerUp::fixedUpdate(float dt) {
    if (isActivated()) {
        m_duration -= dt;

        if (isFinished()) {
            Context::get().eventDispatcher->emit(PowerUpFinished{m_powerUpType});
            logging::Log("PowerUp: {} is finished", toString(m_powerUpType));
        }
        return;
    }

    m_previousPosition = m_position;
    m_position += m_velocity * m_speed * dt;

    glm::vec4 worldAABB = core::getWorldAABB();
    float bottomSide = worldAABB.y;
    if (m_position.y <= bottomSide) {
        m_duration = 0.0f;
        return;
    }
}

bool PowerUp::isActivated() {
    return m_activated;
}

bool PowerUp::isFinished() {
    return m_duration <= 0.0f;
}

void PowerUp::activate() {
    m_activated = true;
}

PowerUpType PowerUp::getType() const {
    return m_powerUpType;
}

Collision PowerUp::checkCollision(GameObject &gameObject) {
    Collision collision = cd::checkCollision(*this, gameObject);
    if (std::get<0>(collision) && !isActivated() && gameObject.getObjectType() == GameObjectType::Player) {
        hide(true);
        Context::get().eventDispatcher->emit(PowerUpActivated{m_powerUpType});
        logging::Log("PowerUp: {} is activated", toString(m_powerUpType));
        activate();
    }
    return collision;
}
