#include "texture_2d.hpp"

#include "glad/glad.h"
#include "render_type.hpp"

bool Texture2D::operator==(const Texture2D &other) const noexcept {
    return m_ID == other.m_ID;
}

void Texture2D::bind() const {
    if (render::type == RenderType::OpenGL) {
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }
}

unsigned int Texture2D::getImageFormat() const noexcept {
    return m_imageFormat;
}

unsigned int Texture2D::getWidth() const noexcept {
    return m_width;
}

unsigned int Texture2D::getHeight() const noexcept {
    return m_height;
}

unsigned int Texture2D::getID() const noexcept {
    return m_ID;
}

void Texture2D::generate(unsigned int width, unsigned int height, bool alpha, unsigned char *data) {
    m_width = width;
    m_height = height;
    if (render::type == RenderType::OpenGL) {
        if (alpha) {
            m_internalFormat = GL_RGBA8;
            m_imageFormat = GL_RGBA;
        } else {
            m_internalFormat = GL_RGB8;
            m_imageFormat = GL_RGB;
        }

        glGenTextures(1, &m_ID);
        glBindTexture(GL_TEXTURE_2D, m_ID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
