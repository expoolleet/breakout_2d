#include "texture_2d.hpp"

#include "glad/glad.h"

Texture2D::Texture2D() {
    m_internalFormat = GL_RGB8;
    m_imageFormat = GL_RGB;
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

unsigned int Texture2D::getImageFormat() const {
    return m_imageFormat;
}

unsigned int Texture2D::getWidth() const {
    return m_width;
}

unsigned int Texture2D::getHeight() const {
    return m_height;
}

void Texture2D::generate(int width, int height, bool alpha, unsigned char *data) {
    if (alpha) {
        m_internalFormat = GL_RGBA8;
        m_imageFormat = GL_RGBA;
    }
    m_width = width;
    m_height = height;
    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}
