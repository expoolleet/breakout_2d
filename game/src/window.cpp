#include "window.hpp"

#include <GLFW/glfw3.h>

std::pair<float, float> Window::getMousePosition() {
    double xpos = 0, ypos = 0;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    return {xpos, ypos};
}

GLFWwindow *Window::createWindow(std::string name, int width, int height) {
    m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    m_width = width;
    m_height = height;
    return m_window;
}
