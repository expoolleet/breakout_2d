#pragma once

#include <glm/glm.hpp>

#include "object_2d.hpp"
#include "shader.hpp"

class Background : public Object2D {
   private:
    ShaderView m_shader = nullptr;
};
