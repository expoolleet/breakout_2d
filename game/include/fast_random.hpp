#pragma once

#include <random>

#include "custom_attributes.hpp"

// Fast Random namespace
namespace fastrand {
inline std::mt19937 &getEngine() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

inline std::uniform_real_distribution<float> &getRealDist() {
    static std::uniform_real_distribution<float> realDist{0.0f, 1.0f};
    return realDist;
}

inline static float randomFloatInRange(float a, float b) {
    std::uniform_real_distribution<float> distribution{a, b};
    return distribution(getEngine());
}

inline static int randomIntInRange(int a, int b) {
    std::uniform_int_distribution<int> distribution{a, b};
    return distribution(getEngine());
}

inline static unsigned int randomUIntInRange(unsigned int a, unsigned int b) {
    std::uniform_int_distribution<unsigned int> distribution{a, b};
    return distribution(getEngine());
}

inline static float random() {
    return getRealDist()(getEngine());
}

}  // namespace fastrand
