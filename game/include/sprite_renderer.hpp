#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "texture_2d.hpp"

class SpriteRenderer {
   private:
    unsigned int m_VAO;
    void _initRenderData();

   public:
    SpriteRenderer();
    ~SpriteRenderer() noexcept;
    void drawSprite(ShaderPtr shader, Texture2DPtr texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec4 color);
};

using SpriteRendererPtr = observer_ptr<SpriteRenderer>;
