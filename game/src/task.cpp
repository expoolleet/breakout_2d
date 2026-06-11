#include "task.hpp"

Task Task::promise_type::get_return_object() noexcept {
    return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
}

std::suspend_always Task::promise_type::initial_suspend() noexcept {
    return {};
}

std::suspend_always Task::promise_type::final_suspend() noexcept {
    return {};
}

void Task::promise_type::return_void() {}

void Task::promise_type::unhandled_exception() {
    std::rethrow_exception(std::move(std::current_exception()));
}

Task::Task(std::coroutine_handle<promise_type> h) : handle(h) {}

Task::~Task() noexcept {
    if (handle) {
        handle.destroy();
    }
}

Task::Task(Task &&other) noexcept : handle(other.handle) {
    other.handle = nullptr;
}

Task &Task::operator=(Task &&other) noexcept {
    if (this != &other) {
        if (handle != nullptr) handle.destroy();
        handle = other.handle;
        other.handle = nullptr;
    }
    return *this;
}

bool Task::isDone() const {
    return handle.done();
}

void Task::resume() const {
    if (!handle.done()) handle.resume();
}
