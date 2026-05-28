#include "background.hpp"

#include "engine_context.hpp"
#include "game_core.hpp"

Background::Background(ContextPtr context, Texture2DPtr texture) : Object2D(context, texture) {
    m_position = glm::vec2(core::getWorldAABB().x, core::getWorldAABB().y);
    m_size = glm::vec2(core::getWorldWidth(), core::getWorldHeight());
}
