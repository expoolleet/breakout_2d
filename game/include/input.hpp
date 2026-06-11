#pragma once

#include "GLFW/glfw3.h"
#include "logging.hpp"

inline constexpr int MAX_KEY_CODE = 1024;

inline constexpr int KEY_A = GLFW_KEY_A;
inline constexpr int KEY_D = GLFW_KEY_D;
inline constexpr int KEY_SPACE = GLFW_KEY_SPACE;
inline constexpr int KEY_ENTER = GLFW_KEY_ENTER;

class Keys {
   public:
    std::array<bool, MAX_KEY_CODE> keys = {false};
    std::array<bool, MAX_KEY_CODE> prevKeys = {false};

    inline void update() noexcept {
        std::copy(keys.begin(), keys.begin() + MAX_KEY_CODE, prevKeys.data());
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
