#pragma once

#include <format>
#include <iostream>

namespace _log {
template <typename... Args> inline void Log(std::format_string<Args...> message, Args &&...args) {
    std::cout << "INFO: " + std::format(message, std::forward<Args>(args)...) << "\n";
}

template <typename... Args> inline void LogSilent(std::format_string<Args...> message, Args &&...args) {
    std::cout << std::format(message, std::forward<Args>(args)...) << "\n";
}

template <typename... Args> inline void Warn(std::format_string<Args...> message, Args &&...args) {
    std::cout << "WARNING: " << std::format(message, std::forward<Args>(args)...) << "\n";
}

template <typename... Args> inline void Error(std::format_string<Args...> message, Args &&...args) {
    std::clog << "ERROR: " << std::format(message, std::forward<Args>(args)...) << "\n";
}
} // namespace _log
