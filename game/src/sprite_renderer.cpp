#include "sprite_renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glad/glad.h"
#include "render_type.hpp"
#include "shader.hpp"
#include "texture_2d.hpp"

void SpriteRenderer::_initRenderData() {
    static float vertexData[16] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};
    static int indices[6] = {0, 1, 2, 2, 3, 0};

    if (render::type == RenderType::OpenGL) {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData[0], GL_STATIC_DRAW);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)0);
        glBindVertexArray(0);
    }
}

SpriteRenderer::SpriteRenderer() {
    _initRenderData();
}

SpriteRenderer::~SpriteRenderer() noexcept {
    if (render::type == RenderType::OpenGL) {
        glDeleteVertexArrays(1, &m_VAO);
    }
}

void SpriteRenderer::drawSprite(Shader &shader, const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate,
                                glm::vec4 color) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    shader.setMat4("model", model);
    shader.setVec4("spriteColor", color);

    texture.bind();
    if (render::type == RenderType::OpenGL) {
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    }
}
