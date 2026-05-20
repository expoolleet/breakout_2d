#include "background.hpp"

#include "engine_context.hpp"
#include "game_core.hpp"
#include "texture_literals.hpp"

Background::Background(Texture2DRef texture) : Object2D(texture) {
    m_position = glm::vec2(core::getWorldAABB().x, core::getWorldAABB().y);
    m_size = glm::vec2(core::getWorldWidth(), core::getWorldHeight());
}
