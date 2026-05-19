#include "fps_counter.hpp"

#include <chrono>

using namespace std::literals;

void FPSCounter::update() {
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>((now - lastTime)) >= 1s) {
        lastTime = now;
        m_fps = m_frameCount;
        m_frameCount = 0;
    }
    ++m_frameCount;
}

int FPSCounter::getFPS() const noexcept {
    return m_fps;
}
