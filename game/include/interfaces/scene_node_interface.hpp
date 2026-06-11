#pragma once

#include "observer_ptr.hpp"
#include "scene_node.hpp"

// Non-virtual interface pattern
template <typename T>
class SceneNodeInterface {
   public:
    SceneNodeInterface() = default;
    SceneNodeInterface(const SceneNodeInterface<T> &) = default;
    SceneNodeInterface &operator=(const SceneNodeInterface<T> &) = default;
    SceneNodeInterface(SceneNodeInterface<T> &&) = default;
    SceneNodeInterface &operator=(SceneNodeInterface<T> &&) = default;
    virtual ~SceneNodeInterface() noexcept = default;

    inline virtual void setParent(observer_ptr<T> parent) {
        assert(static_cast<T *>(this) != parent.get() && "Cannot set parent to self");
        auto oldParent = m_node.m_parent;
        m_node.m_parent = parent;
        if (oldParent) {
            oldParent->_removeChildUnsafe(static_cast<T *>(this));
        }
        if (m_node.m_parent) {
            m_node.m_parent->_addChildUnsafe(static_cast<T *>(this));
        }
    }

    inline observer_ptr<T> getParent() const noexcept {
        return m_node.m_parent;
    }

    inline virtual void addChild(observer_ptr<T> child) {
        if (!child) return;
        child->setParent(static_cast<T *>(this));
    }

    inline std::vector<observer_ptr<T>> &getChildren() noexcept {
        return m_node.m_children;
    }

    inline void clearChildren() noexcept {
        if (m_node.m_children.empty()) return;
        auto copy = m_node.m_children;
        for (auto &child : copy) {
            child->setParent(nullptr);
        }
    }

   protected:
    SceneNode<T> m_node;

    inline void _addChildUnsafe(observer_ptr<T> child) {
        auto it = std::find(m_node.m_children.begin(), m_node.m_children.end(), child);
        if (it == m_node.m_children.end()) {
            m_node.m_children.push_back(child);
        }
    }

    inline void _removeChildUnsafe(observer_ptr<T> child) {
        auto it = std::find(m_node.m_children.begin(), m_node.m_children.end(), child);
        if (it != m_node.m_children.end()) {
            m_node.m_children.erase(it);
        }
    }
};
