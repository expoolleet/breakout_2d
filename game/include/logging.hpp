#pragma once

#include <format>
#include <glm/glm.hpp>
#include <iostream>

#include "custom_attributes.hpp"
#include "glm_stringify.hpp"

namespace logging {
NO_DESTROY_ATTR inline std::string m_lastInfoMessage;
NO_DESTROY_ATTR inline std::string m_lastWarnMessage;
NO_DESTROY_ATTR inline std::string m_lastErrorMessage;

using glm::to_string;
using std::to_string;

template <typename... Args>
inline void Info(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    if (m_lastInfoMessage == formatedMessage) {
        return;
    }
    m_lastInfoMessage = formatedMessage;
    std::cout << "INFO: " + formatedMessage << "\n";
}

template <typename T>
inline void Info(const T &value) {
    std::string message;
    if constexpr (std::is_convertible_v<T, std::string_view>) {
        message = value;
    } else {
        message = to_string(value);
    }

    if (m_lastInfoMessage == message) {
        return;
    }
    m_lastInfoMessage = message;
    std::cout << "INFO: " + message << "\n";
}

template <typename... Args>
inline void InfoSilent(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    std::cout << formatedMessage << "\n";
}

template <typename T>
inline void InfoSilent(const T &value) {
    std::string message;
    if constexpr (std::is_convertible_v<T, std::string_view>) {
        message = value;
    } else {
        message = to_string(value);
    }
    std::cout << message << "\n";
}

template <typename... Args>
inline void Warn(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    if (m_lastWarnMessage == formatedMessage) {
        return;
    }
    m_lastWarnMessage = formatedMessage;
    std::cout << "WARNING: " + formatedMessage << "\n";
}

template <typename... Args>
inline void Error(std::format_string<Args...> message, Args &&...args) {
    auto formatedMessage = std::format(message, std::forward<Args>(args)...);
    if (m_lastErrorMessage == formatedMessage) {
        return;
    }
    m_lastErrorMessage = formatedMessage;
    std::cerr << "ERROR: " + formatedMessage << "\n";
}
}  // namespace logging
