#pragma once

#include <coroutine>

struct Task {
    struct promise_type {
        Task get_return_object() noexcept;
        std::suspend_always initial_suspend() noexcept;
        std::suspend_always final_suspend() noexcept;
        void return_void();
        void unhandled_exception();
    };

    std::coroutine_handle<promise_type> handle = nullptr;

    Task(std::coroutine_handle<promise_type> h);
    ~Task() noexcept;

    Task(const Task &) = delete;
    Task operator=(const Task &) = delete;

    Task(Task &&) noexcept;
    Task &operator=(Task &&) noexcept;

    bool isDone() const;
    void resume() const;
};
