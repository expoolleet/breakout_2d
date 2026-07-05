#pragma once

#include "custom_attributes.hpp"
#include "easing_functions.hpp"
#include "game_core.hpp"

inline constexpr int TWEEN_ADDITIONAL_ALLOC_COUNT = 2;

using TweenValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4>;

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
};

class Tween {
    using TweenPtr = observer_ptr<Tween>;

   public:
    static void allocTween(int tweenCount);
    static TweenPtr createTween();
    static void updateAll(float delta);

    TweenTarget &tweenProperty(std::function<void(TweenValue value)> updateFunction, TweenValue startValue, TweenValue endValue,
                               float time);
    void tweenCallback(std::function<void()> callback);
    bool isRunning();
    bool hasTargets();
    void play();
    void stop();
    void loop();
    void reverse();
    void parallel();
    void finish();

   private:
    Tween() = default;
    NO_DESTROY_ATTR static std::vector<std::unique_ptr<Tween>> m_tweens;
    std::function<void()> m_callback;
    std::vector<TweenTarget> m_targets;

    bool m_isRunning = false;
    bool m_loopingEnaled = false;
    bool m_reverseEnabled = false;
    bool m_parallelEnabled = false;
    int m_currentStep = 0;
    uint64_t m_id;

    static void _checkTween(TweenPtr tween);
    void _clearFinishedTargets();
    void _update(TweenTarget &target, float delta);
};

using TweenPtr = observer_ptr<Tween>;
