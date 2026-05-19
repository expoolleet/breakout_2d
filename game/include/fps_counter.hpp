#pragma once

class FPSCounter {
   private:
    int m_frameCount = 0;
    int m_fps = 0;

   public:
    FPSCounter() = default;
    void update();
    int getFPS() const noexcept;
};
