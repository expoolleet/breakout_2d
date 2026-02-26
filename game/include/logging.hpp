#pragma once

#include "custom_attributes.hpp"

#include <format>
#include <iostream>

namespace logging {
NO_DESTROY_ATTR inline std::string m_lastInfoMessage;
NO_DESTROY_ATTR inline std::string m_lastWarnMessage;
NO_DESTROY_ATTR inline std::string m_lastErrorMessage;

template <typename... Args> inline void Log(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    if (m_lastInfoMessage == formatedMessage) {
        return;
    }
    m_lastInfoMessage = formatedMessage;
    std::cout << "INFO: " + formatedMessage << "\n";
}

template <typename... Args> inline void LogSilent(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    m_lastInfoMessage = formatedMessage;
    std::cout << formatedMessage << "\n";
}

template <typename... Args> inline void Warn(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    if (m_lastWarnMessage == formatedMessage) {
        return;
    }
    m_lastWarnMessage = formatedMessage;
    std::cout << "WARNING: " + formatedMessage << "\n";
}

template <typename... Args> inline void Error(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    if (m_lastErrorMessage == formatedMessage) {
        return;
    }
    m_lastErrorMessage = formatedMessage;
    std::cerr << "ERROR: " + formatedMessage << "\n";
}
} // namespace logging
