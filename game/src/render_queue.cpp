#include "render_queue.hpp"

void RenderQueue::push(const SpriteCommand &command) {
    m_commands.push_back(command);
}

void RenderQueue::sort() {
    std::sort(m_commands.begin(), m_commands.end(),
              [](const SpriteCommand &lhs, const SpriteCommand &rhs) { return lhs.layer < rhs.layer; });
}

const SpriteCommands &RenderQueue::getCommands() const {
    return m_commands;
}

void RenderQueue::clear() {
    m_commands.clear();
}
