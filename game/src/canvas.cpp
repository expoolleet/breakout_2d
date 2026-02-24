#include "canvas.hpp"

#include "logging.hpp"
#include "shader.hpp"

void Canvas::init() {
    if (m_initialized) {
        logging::Warn("Could not initialize Canvas VAO because it is already initialized");
        return;
    }
    m_initialized = true;

    static float vertices[16] = {1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    static int indices[6] = {0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    unsigned int vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    unsigned int ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(0));
    glBindVertexArray(0);
}
void Canvas::render(Shader &shader, std::vector<unsigned int> &colorAttachments) {
    for (unsigned int i = 0; i < colorAttachments.size(); ++i) {
        glBindTextureUnit(i, colorAttachments[i]);
    }
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
