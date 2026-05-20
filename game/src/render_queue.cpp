#include "render_queue.hpp"

void RenderQueue::push(const SpriteCommand &command) {
    m_commands.push_back(command);
}

void RenderQueue::sort() {
    std::stable_sort(m_commands.begin(), m_commands.end(), [](const SpriteCommand &lhs, const SpriteCommand &rhs) {
        if (lhs.layer != rhs.layer) return lhs.layer < rhs.layer;
        if (lhs.shader != rhs.shader) return lhs.shader->getID() < rhs.shader->getID();
        return lhs.texture->getID() < rhs.texture->getID();
    });
}

const SpriteCommands &RenderQueue::getCommands() const {
    return m_commands;
}

void RenderQueue::clear() {
    m_commands.clear();
}
