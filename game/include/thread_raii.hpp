#pragma once

class ThreadRAII {
   public:
    enum class DtorAction {
        Join,
        Detach
    };

    ThreadRAII() = default;
    ThreadRAII(std::thread &&t, DtorAction a) : m_action(a), m_t(std::move(t)) {}

    ThreadRAII(const ThreadRAII &) = delete;
    ThreadRAII &operator=(const ThreadRAII &) = delete;

    ThreadRAII(ThreadRAII &&other) {
        if (this != &other) {
            m_t = std::move(other.m_t);
            m_action = other.m_action;
        }
    }

    ThreadRAII &operator=(ThreadRAII &&other) {
        if (this != &other) {
            m_t = std::move(other.m_t);
            m_action = other.m_action;
        }
        return *this;
    }

    ~ThreadRAII() {
        if (m_t.joinable()) {
            if (m_action == DtorAction::Join) {
                m_t.join();
            } else {
                m_t.detach();
            }
        }
    }

    inline std::thread &get() {
        return m_t;
    }

   private:
    DtorAction m_action;
    std::thread m_t;
};
