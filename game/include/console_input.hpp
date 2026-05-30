#pragma once

#include <iostream>

#ifdef _WIN32
#include <conio.h>
#endif

inline bool isTyping() {
#ifdef _WIN32
    return _kbhit();
#endif
}

inline char getTypedKey() {
#ifdef _WIN32
    return _getch();
#endif
}

inline bool isEnterPressed(char key) {
    return key == '\r';
}

inline bool isBackspacePressed(char key) {
    return key == '\b';
}

inline void clearInput(size_t size) {
    std::cout << (std::string(size + 1, ' ') + "\r");
}
