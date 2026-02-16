#pragma once

#include <format>
#include <iostream>

namespace _log {
inline std::string m_lastInfoMessage;
inline std::string m_lastWarnMessage;
inline std::string m_lastErrorMessage;

template <typename... Args> inline void Log(std::format_string<Args...> message, Args &&...args) {
    if (m_lastInfoMessage == message.get()) {
        return;
    }
    m_lastInfoMessage = message.get();
    std::cout << "INFO: " + std::format(message, std::forward<Args>(args)...) << "\n";
}

template <typename... Args> inline void LogSilent(std::format_string<Args...> message, Args &&...args) {
    if (m_lastInfoMessage == message.get()) {
        return;
    }
    m_lastInfoMessage = message.get();
    std::cout << std::format(message, std::forward<Args>(args)...) << "\n";
}

template <typename... Args> inline void Warn(std::format_string<Args...> message, Args &&...args) {
    if (m_lastWarnMessage == message.get()) {
        return;
    }
    m_lastWarnMessage = message.get();
    std::cout << "WARNING: " << std::format(message, std::forward<Args>(args)...) << "\n";
}

template <typename... Args> inline void Error(std::format_string<Args...> message, Args &&...args) {
    if (m_lastErrorMessage == message.get()) {
        return;
    }
    m_lastErrorMessage = message.get();
    std::clog << "ERROR: " << std::format(message, std::forward<Args>(args)...) << "\n";
}
} // namespace _log
