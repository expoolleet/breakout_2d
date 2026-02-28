#pragma once

class Window {
   private:
    inline static int m_width = 800;
    inline static int m_height = 600;
    inline static GLFWwindow *m_window = nullptr;

    // Removing default constructor
    Window() = delete;
    // Removing copy constructor
    Window(const Window &) = delete;
    // Removing the assignment operator
    Window &operator=(const Window &) = delete;

   public:
    inline static int getWidth() {
        return m_width;
    }

    inline static int getHeight() {
        return m_height;
    }

    inline static void setWidth(int width) {
        m_width = width;
    }

    inline static void setHeight(int height) {
        m_height = height;
    }

    inline static float getAspectRatio() {
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }

    static std::pair<float, float> getMousePosition();

    static GLFWwindow *createWindow(std::string name, int width, int height);
};
