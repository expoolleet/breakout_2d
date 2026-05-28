#pragma once

namespace timer {

inline float time() {
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
    return static_cast<float>(time) / 1000.0f;
}

}  // namespace timer
