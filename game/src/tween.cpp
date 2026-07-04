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

TweenPtr Tween::createTween() {
    auto it = std::find_if(m_tweens.begin(), m_tweens.end(), [](std::unique_ptr<Tween> &t) { return !t->m_isRunning; });
    if (it != m_tweens.end()) {
        logging::Info("Tween: reused tween {}", it->get()->m_id);
        return TweenPtr(it->get());
    }
    allocTween(TWEEN_ADDITIONAL_ALLOC_COUNT);
    return TweenPtr(m_tweens.back().get());
}

void Tween::updateAll(float delta) {
    for (auto &tween : m_tweens) {
        if (!tween->isRunning()) continue;
        for (auto &target : tween->m_targets) {
            tween->update(target, delta);
        }
        _checkTween(tween.get());
    }
}

void Tween::tweenProperty(std::function<void(TweenValue value)> updateFunction, TweenValue startValue, TweenValue endValue, float time) {
    m_targets.emplace_back(updateFunction, startValue, endValue, time);
    m_isRunning = true;
}

void Tween::tweenCallback(std::function<void()> callback) {
    m_callback = callback;
}

bool Tween::isRunning() {
    return m_isRunning;
}

bool Tween::hasTargets() {
    return m_targets.empty();
}

void Tween::play() {
    m_isRunning = true;
}

void Tween::stop() {
    m_isRunning = false;
}

void Tween::loop() {
    m_isLooping = true;
}

void Tween::reverse() {
    m_isReversing = true;
}

void Tween::update(TweenTarget &target, float delta) {
    target.time = std::min(target.time + delta, target.endTime);
    float t = target.endTime > 0.0f ? (target.time / target.endTime) : 1.0f;

    std::visit(
        [&](const auto &endValue) {
            using T = std::decay_t<decltype(endValue)>;
            const T &startValue = std::get<T>(target.startValue);
            TweenValue interpolatedValue = core::lerp(startValue, endValue, t);
            target.updateFunction(interpolatedValue);
        },
        target.endValue);

    if (m_isLooping && target.finished()) {
        target.time = 0.0;
        if (m_isReversing) {
            auto temp = target.endValue;
            target.endValue = target.startValue;
            target.startValue = temp;
        } else {
            target.updateFunction(target.startValue);
        }
    }
}

void Tween::finish() {
    m_isRunning = false;
    m_targets.clear();
    if (m_callback) m_callback();
    m_callback = nullptr;
}

void Tween::_clearFinishedTargets() {
    std::erase_if(m_targets, [](TweenTarget &target) { return target.finished(); });
}

void Tween::_checkTween(TweenPtr tween) {
    if (tween->m_isLooping) return;
    tween->_clearFinishedTargets();
    if (tween->hasTargets()) {
        tween->finish();
        logging::Info("Tween: finished tween {}", tween->m_id);
    }
}

std::vector<std::unique_ptr<Tween>> Tween::m_tweens = {};
