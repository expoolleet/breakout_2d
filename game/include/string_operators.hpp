#pragma once

#include <string>
#include <string_view>

struct string_hash {
    using is_transparent = void;

    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }

    size_t operator()(const std::string &s) const noexcept {
        return std::hash<std::string>{}(s);
    }
};

struct string_view_equal {
    using is_transparent = void;

    bool operator()(std::string_view a, std::string_view b) const noexcept {
        return a == b;
    }
};
