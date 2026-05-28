#pragma once

#include <unordered_map>
#include <vector>

#include "logging.hpp"
#include "object_2d.hpp"

class ObjectManager {
   private:
    ContextPtr m_context;
    std::vector<std::unique_ptr<Object2D>> m_objects;
    std::unordered_map<std::string, Object2DPtr> m_objectMap;

   public:
    ObjectManager(ContextPtr context);

    template <typename T, typename... Args>
    T *create(Args &&...args) {
        m_objects.emplace_back(std::make_unique<T>(m_context, std::forward<Args>(args)...));
        return dynamic_cast<T *>(m_objects.back().get());
    }

    template <typename T, typename... Args>
    T *create(const std::string &name, Args &&...args) {
        if (m_objectMap.contains(name)) {
            logging::Warn("(ObjectManager) Object with name '{}' is already created", name);
            return m_objectMap[name];
        }
        m_objects.emplace_back(std::make_unique<T>(m_context, std::forward<Args>(args)...));
        m_objectMap[name] = m_objects.back().get();
        return dynamic_cast<T *>(m_objects.back().get());
    }

    template <typename T>
    T *get(const std::string &name) {
        auto it = m_objectMap.find(name);
        if (it == m_objectMap.end()) {
            logging::Warn("(ObjectManager) Object with name '{}' not found", name);
            return nullptr;
        }
        return it->second;
    }

    void cleanup();
    size_t size();
    bool contains(const std::string &name);
};

using ObjectManagerPtr = observer_ptr<ObjectManager>;
