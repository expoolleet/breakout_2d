#pragma once
#include <cassert>

template <typename T>
class observer_ptr {
   private:
    T *m_ptr = nullptr;

   public:
    void operator delete(void *) noexcept = delete;
    void operator delete[](void *) noexcept = delete;

    observer_ptr() = default;
    observer_ptr(T *ptr) : m_ptr(ptr) {}

    inline T *operator->() const noexcept {
        assert(m_ptr && "Object's pointer is null!");
        return m_ptr;
    }

    inline T &operator*() const noexcept {
        assert(m_ptr && "Object's pointer is null!");
        return *m_ptr;
    }

    inline explicit operator bool() const noexcept {
        return m_ptr != nullptr;
    }

    inline bool operator==(const observer_ptr<T> &otherPtr) const noexcept {
        return otherPtr.m_ptr == m_ptr;
    }

    inline bool operator!=(const observer_ptr<T> &otherPtr) const noexcept {
        return !(otherPtr.m_ptr == m_ptr);
    }
};
