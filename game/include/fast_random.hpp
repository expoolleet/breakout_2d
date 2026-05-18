#pragma once

#include <random>

#include "custom_attributes.hpp"

// Fast Random namespace
namespace fastrand {

struct XorShiftState {
    uint32_t a = 1u;
};

inline std::mt19937 &getEngine_mt19937() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

inline XorShiftState &getXorShiftState() {  
    static XorShiftState state;
    return state;
}

inline uint32_t xorShift32(XorShiftState &state) {
    uint32_t x = state.a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state.a = x;
    return x;
}

inline std::uniform_real_distribution<float> &getRealDist() {
    static std::uniform_real_distribution<float> realDist{0.0f, 1.0f};
    return realDist;
}

inline float randomFloatInRange(float a, float b) {
    std::uniform_real_distribution<float> distribution{a, b};
    return distribution(getEngine_mt19937());
}

inline int randomIntInRange(int a, int b) {
    std::uniform_int_distribution<int> distribution{a, b};
    return distribution(getEngine_mt19937());
}

inline unsigned int randomUIntInRange(unsigned int a, unsigned int b) {
    std::uniform_int_distribution<unsigned int> distribution{a, b};
    return distribution(getEngine_mt19937());
}

inline static float random() {
    return getRealDist()(getEngine_mt19937());
}

inline float frandom() {
    uint32_t nextUint = xorShift32(getXorShiftState());
    return (nextUint >> 8) * (1.0f / 16777216.0f);
}

inline float frandomFloatInRange(float a, float b) {
    return a + frandom() * (b - a);
}

}  // namespace fastrand
