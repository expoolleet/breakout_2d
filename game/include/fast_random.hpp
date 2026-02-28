#pragma once

#include <random>

inline static std::mt19937 randomEngine_mt19937;

// Fast Random namespace
namespace fastrand {
inline static void initRandomEngine() {
    randomEngine_mt19937.seed(std::random_device()());
}

inline static float randomFloatInRange(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(randomEngine_mt19937);
}

inline static int randomIntInRange(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(randomEngine_mt19937);
}

inline static unsigned int randomUIntInRange(unsigned int min, unsigned int max) {
    std::uniform_int_distribution<unsigned int> distribution(min, max);
    return distribution(randomEngine_mt19937);
}
}  // namespace fastrand
