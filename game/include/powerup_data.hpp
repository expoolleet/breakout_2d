#pragma once

#include "texture_2d.hpp"

struct PowerUpData {
    glm::vec4 color;
    float duration;
    Texture2DPtr texture;

    PowerUpData() : color(glm::vec4(1.0f)), duration(0.0f), texture(nullptr) {}
    PowerUpData(glm::vec4 col, float dur, Texture2DPtr tex) : color(col), duration(dur), texture(tex) {}
};
