#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

using Handler = std::function<void(const void *)>;

class EventDispatcher {
   private:
    std::unordered_map<std::type_index, std::vector<Handler>> m_events;

   public:
    EventDispatcher() = default;

    template <typename T>
    void subscribe(std::function<void(const T &)> callback);

    template <typename T>
    void emit(const T &event);

    void clear();
};

using EventDispatcherPtr = std::unique_ptr<EventDispatcher>;
