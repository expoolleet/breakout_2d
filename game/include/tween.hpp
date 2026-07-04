#pragma once

#include "custom_attributes.hpp"
#include "game_core.hpp"

inline constexpr int TWEEN_ADDITIONAL_ALLOC_COUNT = 2;

using TweenValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4>;

struct TweenTarget {
    std::function<void(TweenValue value)> updateFunction;
    TweenValue startValue;
    TweenValue endValue;
    float endTime;
    float time = 0.0f;

    TweenTarget(std::function<void(TweenValue value)> f, TweenValue sv, TweenValue ev, float t)
        : updateFunction(f), startValue(sv), endValue(ev), endTime(t) {}

    bool finished() {
        return core::isEqualApprox(time, endTime);
    }
};

class Tween {
    using TweenPtr = observer_ptr<Tween>;

   public:
    static void allocTween(int tweenCount);
    static TweenPtr createTween();
    static void updateAll(float delta);

    void tweenProperty(std::function<void(TweenValue value)> updateFunction, TweenValue startValue, TweenValue endValue, float time);
    void tweenCallback(std::function<void()> callback);
    bool isRunning();
    bool hasTargets();
    void play();
    void stop();
    void loop();
    void reverse();
    void update(TweenTarget &target, float delta);
    void finish();

   private:
    Tween() = default;
    NO_DESTROY_ATTR static std::vector<std::unique_ptr<Tween>> m_tweens;
    std::function<void()> m_callback;
    std::vector<TweenTarget> m_targets;

    bool m_isRunning = false;
    bool m_isLooping = false;
    bool m_isReversing = false;
    uint64_t m_id;

    void _clearFinishedTargets();
    static void _checkTween(TweenPtr tween);
};

using TweenPtr = observer_ptr<Tween>;
