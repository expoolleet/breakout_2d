#pragma once

#include "shader.hpp"

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Texture2D; // fwd;

class SpriteRenderer {
  private:
    unsigned int m_VAO;
    void _initRenderData();

  public:
    SpriteRenderer();
    void drawSprite(Shader &shader, const Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f), float rotate = 0.0f,
                    glm::vec4 color = glm::vec4(1.0f));
};
