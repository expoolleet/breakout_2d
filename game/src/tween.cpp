#include "tween.hpp"

#include <algorithm>

#include "logging.hpp"

void Tween::allocTween(int tweenCount) {
    for (int i = 0; i < tweenCount; ++i) {
        Tween *tween = new Tween();
        tween->m_id = m_tweens.size();
        m_tweens.emplace_back(tween);
    }
    logging::Info("Tween: created {} tweens", tweenCount);
}

TweenPtr Tween::createTween(const std::string &name) {
    auto it = std::find_if(m_tweens.begin(), m_tweens.end(), [](std::unique_ptr<Tween> &t) { return !t->hasTargets(); });
    if (it == m_tweens.end()) {
        logging::Info("All tweens are busy, allocating new tweens...");
        allocTween(TWEEN_ADDITIONAL_ALLOC_COUNT);
        return createTween(name);
    } else {
        logging::Info("Tween: reused with id {}", it->get()->m_id);
    }
    TweenPtr tween = TweenPtr(it->get());
    _saveTween(name, tween);
    return tween;
}

TweenPtr Tween::getTween(ID id) {
    assert(id >= 0 && id < m_tweens.size() && "Wrong id");
    return TweenPtr(m_tweens[id].get());
}

TweenPtr Tween::getTween(const std::string &name) {
    auto it = m_tweenMap.find(name);
    if (it == m_tweenMap.end()) {
        logging::Error("Tween with name '{}' is not found!", name);
        return nullptr;
    }
    return it->second;
}

void Tween::updateAll(float delta) {
    for (auto &tween : m_tweens) {
        if (!tween->isRunning()) continue;
        for (auto &target : tween->m_targets) {
            if (tween->m_parallelEnabled) {
                tween->_update(target, delta);
            } else {
                tween->_update(tween->m_targets.front(), delta);
            }
        }
        _checkTween(tween.get());
    }
}

TweenTarget &Tween::tweenProperty(std::function<void(TweenValue value)> updateFunction, TweenValue startValue, TweenValue endValue,
                                  float time) {
    m_targets.emplace_back(updateFunction, startValue, endValue, time);
    m_isRunning = true;
    return m_targets.back();
}

void Tween::tweenCallback(std::function<void()> callback) {
    if (!callback) {
        logging::Error("Tween callback is not set because it is nullptr!");
        return;
    }
    m_callback = callback;
}

bool Tween::isRunning() const noexcept {
    return m_isRunning;
}

bool Tween::hasTargets() const noexcept {
    return !m_targets.empty();
}

void Tween::play() noexcept {
    m_isRunning = true;
}

void Tween::stop() noexcept {
    m_isRunning = false;
}

void Tween::loop() noexcept {
    m_loopingEnaled = true;
}

void Tween::reverse() noexcept {
    m_reverseEnabled = true;
}

void Tween::parallel() noexcept {
    m_parallelEnabled = true;
}

void Tween::finish() noexcept {
    m_isRunning = false;
    m_targets.clear();
    if (m_callback) m_callback();
    m_callback = nullptr;
}

ID Tween::getID() const noexcept {
    return m_id;
}

void Tween::_update(TweenTarget &target, float delta) {
    target.time = std::min(target.time + delta, target.endTime);
    if (target.time < 0.0f) return;
    float t = target.endTime > 0.0f ? (target.time / target.endTime) : 1.0f;

    std::visit(
        [&](const auto &endValue) {
            using T = std::decay_t<decltype(endValue)>;
            const T &startValue = std::get<T>(target.startValue);
            TweenValue interpolatedValue = core::lerp(startValue, endValue, ease(target.easingType, t));
            target.updateFunction(interpolatedValue);
        },
        target.endValue);

    if (m_loopingEnaled && target.finished()) {
        target.time = 0.0f;
        if (m_reverseEnabled) {
            auto temp = target.endValue;
            target.endValue = target.startValue;
            target.startValue = temp;
        } else {
            target.updateFunction(target.startValue);
        }
    }
}

void Tween::_saveTween(const std::string &name, TweenPtr tween) {
    if (name.empty()) {
        return;
    }
    if (m_tweenMap.contains(name)) {
        logging::Error("Tween with name '{}' is already saved, choose another one!", name);
        return;
    }
    m_tweenMap[name] = tween;
}

void Tween::_clearFinishedTargets() {
    std::erase_if(m_targets, [](TweenTarget &target) { return target.finished(); });
}

void Tween::_checkTween(TweenPtr tween) {
    if (tween->m_loopingEnaled) return;
    tween->_clearFinishedTargets();
    if (!tween->hasTargets()) {
        tween->finish();
        logging::Info("Tween: finished tween {}", tween->m_id);
    }
}
