#pragma once

#include <random>

inline static std::mt19937 randomEngine_mt19937;

// Fast Random namespace
namespace _fr {
	inline static void initRandomEngine() {
		randomEngine_mt19937.seed(std::random_device()());
	}

	inline static float randomFloatInRange(float min, float max) {
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(randomEngine_mt19937);
	}
}