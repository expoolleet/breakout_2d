#pragma once

#include <cmath>

inline constexpr float PI = 3.1415926535897932f;

enum class EasingType {
    Linear,
    QuadIn,
    QuadOut,
    QuadIntOut,
    CubicIn,
    CubicOut,
    CubicIntOut,
    SineIn,
    SineOut,
    SineInOut,
    ExpoIn,
    ExpoOut,
    ExpoInOut,
};

inline float easeInCubic(float x) {
    return x * x * x;
}

inline float easeOutCubic(float x) {
    return 1.0f - std::powf(1.0f - x, 3.0f);
}

inline float easeInOutCubic(float x) {
    return x < 0.5f ? 4.0f * x * x * x : 1.0f - std::powf(-2.0f * x + 2.0f, 3) / 2.0f;
}

inline float easeInExpo(float x) {
    return x == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * x - 10.0f);
}

inline float easeOutExpo(float x) {
    return x == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * x);
}

inline float easeInOutExpo(float x) {
    return x == 0.0f   ? 0.0f
           : x == 1.0f ? 1.0f
           : x < 0.5f  ? std::powf(2.0f, 20.0f * x - 10.0f) / 2.0f
                       : (2.0f - std::powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;
}

inline float easeInQuad(float x) {
    return x * x;
}

inline float easeOutQuad(float x) {
    return 1.0f - std::powf(1.0f - x, 2.0f);
}

inline float easeInOutQuad(float x) {
    return x < 0.5f ? 2.0f * x * x : 1.0f - std::powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;
}

inline float easeInSine(float x) {
    return 1.0f - std::cosf((x * PI) / 2.0f);
}

inline float easeOutSine(float x) {
    return std::sinf((x * PI) / 2.0f);
}

inline float easeInOutSine(float x) {
    return -(std::cosf(PI * x) - 1.0f) / 2.0f;
}

inline float ease(EasingType easeType, float x) {
    switch (easeType) {
        case EasingType::Linear:
            return x;
        case EasingType::QuadIn:
            return easeInQuad(x);
        case EasingType::QuadOut:
            return easeOutQuad(x);
        case EasingType::QuadIntOut:
            return easeInOutQuad(x);
        case EasingType::CubicIn:
            return easeInCubic(x);
        case EasingType::CubicOut:
            return easeOutCubic(x);
        case EasingType::CubicIntOut:
            return easeInOutCubic(x);
        case EasingType::SineIn:
            return easeInSine(x);
        case EasingType::SineOut:
            return easeOutSine(x);
        case EasingType::SineInOut:
            return easeInOutSine(x);
        case EasingType::ExpoIn:
            return easeInExpo(x);
        case EasingType::ExpoOut:
            return easeOutExpo(x);
        case EasingType::ExpoInOut:
            return easeInOutExpo(x);
        default:
            return 0;
    }
}
