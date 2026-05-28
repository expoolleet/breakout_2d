#pragma once

#include "GLFW/glfw3.h"
#include "logging.hpp"

constexpr int MAX_KEY_CODE = 1024;

struct Keys {
    bool keys[MAX_KEY_CODE] = {false};
    void press(int key) noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code when pressed: {}", key);
            return;
        }
        keys[key] = true;
    }
    void unpress(int key) noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code when unpressed: {}", key);
            return;
        }
        keys[key] = false;
    }

    bool isPressed(int key) const noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code: {}", key);
            return false;
        }
        return keys[key];
    }
};

constexpr int KEY_A = GLFW_KEY_A;
constexpr int KEY_D = GLFW_KEY_D;
constexpr int KEY_SPACE = GLFW_KEY_SPACE;
constexpr int KEY_ENTER = GLFW_KEY_ENTER;
