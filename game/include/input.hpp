#pragma once

#include "GLFW/glfw3.h"
#include "logging.hpp"

constexpr int MAX_KEY_CODE = 1024;

constexpr int KEY_A = GLFW_KEY_A;
constexpr int KEY_D = GLFW_KEY_D;
constexpr int KEY_SPACE = GLFW_KEY_SPACE;
constexpr int KEY_ENTER = GLFW_KEY_ENTER;

class Keys {
   public:
    bool keys[MAX_KEY_CODE] = {false};
    bool prevKeys[MAX_KEY_CODE] = {false};

    inline void update() noexcept {
        std::copy(keys, keys + MAX_KEY_CODE, prevKeys);
    }

    inline void press(int key) noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code when pressed: {}", key);
            return;
        }
        keys[key] = true;
    }
    inline void unpress(int key) noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code when unpressed: {}", key);
            return;
        }
        keys[key] = false;
    }

    inline bool justPressed(int key) const noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code: {}", MAX_KEY_CODE);
            return false;
        }
        return keys[key] && !prevKeys[key];
    }

    inline bool justReleased(int key) const noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code: {}", MAX_KEY_CODE);
            return false;
        }
        return !keys[key] && prevKeys[key];
    }

    inline bool isPressed(int key) const noexcept {
        if (key < 0 || key >= MAX_KEY_CODE) {
            logging::Error("Wrong key code: {}", key);
            return false;
        }
        return keys[key];
    }
};
