#pragma once

namespace glm {
inline std::string to_string(glm::vec2 vector) {
    return std::format("({}, {})", vector.x, vector.y);
}

inline std::string to_string(glm::vec3 vector) {
    return std::format("({}, {}, {})", vector.x, vector.y, vector.z);
}

inline std::string to_string(glm::vec4 vector) {
    return std::format("({}, {}, {}, {})", vector.x, vector.y, vector.z, vector.w);
}

inline std::string to_string(glm::mat3 mat) {
    return std::format("[[{}, {}, {}]\n[{}, {}, {}]\n[{}, {}, {}]\n]", mat[0][0], mat[0][1], mat[0][2], mat[1][0], mat[1][1], mat[1][2],
                       mat[2][0], mat[2][1], mat[2][2]);
}

inline std::string to_string(glm::mat4 mat) {
    return std::format("[[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n]", mat[0][0], mat[0][1], mat[0][2],
                       mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0],
                       mat[3][1], mat[3][2], mat[3][3]);
}
};  // namespace glm
