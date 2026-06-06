#pragma once

#include "observer_ptr.hpp"

template <typename T>
struct SceneNode {
    observer_ptr<T> m_parent;
    std::vector<observer_ptr<T>> m_children;
};
