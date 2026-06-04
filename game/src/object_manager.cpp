#include "object_manager.hpp"

ObjectManager::ObjectManager(ContextPtr context) : m_context(context) {}

void ObjectManager::cleanup() {
    int count = 0;
    std::erase_if(m_objectMap, [](const auto &kv) { return !kv.second->isAlive(); });
    std::erase_if(m_objects, [&count](const std::unique_ptr<Object2D> &object) {
        if (!object->isAlive()) {
            ++count;
            return true;
        }
        return false;
    });
    logging::Info("(ObjectManager) Erased {} objects", count);
    logging::Info("(ObjectManager) Objects left {}", m_objects.size());
}

size_t ObjectManager::size() {
    return m_objects.size();
}

bool ObjectManager::contains(const std::string &name) {
    return m_objectMap.contains(name);
}
