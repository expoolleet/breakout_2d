#include "object_manager.hpp"

ObjectManager::ObjectManager(ContextPtr context) : m_context(context) {}

void ObjectManager::cleanup() {
    std::erase_if(m_objectMap, [](const auto &kv) { return !kv.second->isAlive(); });
    std::erase_if(m_objects, [](const std::unique_ptr<Object2D> &object) { return !object->isAlive(); });
}

size_t ObjectManager::size() {
    return m_objects.size();
}

bool ObjectManager::contains(const std::string &name) {
    return m_objectMap.contains(name);
}
