#pragma once

class Window {
   private:
    inline static int m_width;
    inline static int m_height;
    inline static GLFWwindow *m_window = nullptr;

    Window() = delete;
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

   public:
    inline static int getWidth() noexcept {
        return m_width;
    }

    inline static int getHeight() noexcept {
        return m_height;
    }

    inline static void setWidth(int width) noexcept {
        m_width = width;
    }

    inline static void setHeight(int height) noexcept {
        m_height = height;
    }

    inline static float getAspectRatio() {
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }

    static std::pair<float, float> getMousePosition();

    static GLFWwindow *createWindow(std::string name, int width, int height);
};
