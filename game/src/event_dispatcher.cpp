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

template<typename T>
void EventDispatcher::subscribe(std::function<void(const T &)> callback) {
	// type erasure
	auto wrapper = [callback](const void *eventData) {
		callback(*static_cast<const T *>(eventData));
	};
	m_events[std::type_index(typeid(T))].push_back(wrapper);
}

template<typename T>
void EventDispatcher::emit(const T &eventData) {
	auto it = m_events.find(typeid(T));
	if (it == m_events.end()) {
		Logging::Warn(std::format("No event has been registered with name {}", typeid(T).name()));
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
