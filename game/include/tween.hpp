#pragma once

#include <unordered_map>
#include <vector>

#include "custom_attributes.hpp"
#include "easing_functions.hpp"
#include "game_core.hpp"

inline constexpr int TWEEN_ADDITIONAL_ALLOC_COUNT = 2;

using TweenValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4>;

using namespace core;

struct TweenTarget {
    std::function<void(TweenValue value)> updateFunction;
    TweenValue startValue;
    TweenValue endValue;
    float endTime;
    float time;
    EasingType easingType;

    TweenTarget(std::function<void(TweenValue value)> f, TweenValue sv, TweenValue ev, float t)
        : updateFunction(f), startValue(sv), endValue(ev), endTime(t), time(0.0f), easingType(EasingType::Linear) {}

    bool finished() {
        return core::isEqualApprox(time, endTime);
    }

    TweenTarget &setEase(EasingType easing) {
        easingType = easing;
        return *this;
    }

    TweenTarget &setDelay(float delay) {
        time -= delay;
        return *this;
    }
};

class Tween {
    using TweenPtr = observer_ptr<Tween>;
    using TweenList = std::vector<std::unique_ptr<Tween>>;
    using TweenMap = std::unordered_map<std::string, TweenPtr>;

   public:
    static void allocTween(int tweenCount);
    static TweenPtr createTween(const std::string &name = "");
    static TweenPtr getTween(ID id);
    static TweenPtr getTween(const std::string &name);
    static void updateAll(float delta);

    TweenTarget &tweenProperty(std::function<void(TweenValue value)> updateFunction, TweenValue startValue, TweenValue endValue,
                               float time);
    void tweenCallback(std::function<void()> callback);
    bool isRunning() const noexcept;
    bool hasTargets() const noexcept;
    void play() noexcept;
    void stop() noexcept;
    void loop() noexcept;
    void reverse() noexcept;
    void parallel() noexcept;
    void finish() noexcept;
    ID getID() const noexcept;

   private:
    Tween() = default;
    NO_DESTROY_ATTR inline static TweenList m_tweens;
    NO_DESTROY_ATTR inline static TweenMap m_tweenMap;
    std::function<void()> m_callback;
    std::vector<TweenTarget> m_targets;

    bool m_isRunning = false;
    bool m_loopingEnaled = false;
    bool m_reverseEnabled = false;
    bool m_parallelEnabled = false;
    int m_currentStep = 0;
    ID m_id;

    static void _checkTween(TweenPtr tween);
    static void _saveTween(const std::string &name, TweenPtr tween);
    void _clearFinishedTargets();
    void _update(TweenTarget &target, float delta);
};

using TweenPtr = observer_ptr<Tween>;
