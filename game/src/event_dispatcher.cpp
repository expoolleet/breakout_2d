#include "event_dispatcher.hpp"

#include "event_type.hpp"
#include "logging.hpp"

#include <format>
#include <functional>
#include <typeindex>

EventDispatcher &EventDispatcher::Get() {
    static EventDispatcher dispatcher;
    return dispatcher;
}

template <typename T> void EventDispatcher::subscribe(std::function<void(const T &)> callback) {
    // type erasure
    auto wrapper = [callback](const void *eventData) { callback(*static_cast<const T *>(eventData)); };
    m_events[std::type_index(typeid(T))].push_back(wrapper);
}

template <typename T> void EventDispatcher::emit(const T &eventData) {
    auto it = m_events.find(typeid(T));
    if (it == m_events.end()) {
        logging::Warn("No event has been registered with name {}", typeid(T).name());
        return;
    }
    for (auto &callback : it->second) {
        callback(&eventData);
    }
}

void EventDispatcher::clear() {
    m_events.clear();
}

template void EventDispatcher::subscribe<BallFliedOff>(std::function<void(const BallFliedOff &)> callback);
template void EventDispatcher::emit<BallFliedOff>(const BallFliedOff &eventData);

template void EventDispatcher::subscribe<PlayerMoved>(std::function<void(const PlayerMoved &)> callback);
template void EventDispatcher::emit<PlayerMoved>(const PlayerMoved &eventData);

template void EventDispatcher::subscribe<PowerUpActivated>(std::function<void(const PowerUpActivated &)> callback);
template void EventDispatcher::emit<PowerUpActivated>(const PowerUpActivated &eventData);

template void EventDispatcher::subscribe<PowerUpFinished>(std::function<void(const PowerUpFinished &)> callback);
template void EventDispatcher::emit<PowerUpFinished>(const PowerUpFinished &eventData);

template void EventDispatcher::subscribe<BallHit>(std::function<void(const BallHit &)> callback);
template void EventDispatcher::emit<BallHit>(const BallHit &eventData);
