#pragma once

class Texture2D {
   private:
    unsigned int m_ID = 0;
    unsigned int m_width = 0;
    unsigned int m_height = 0;
    unsigned int m_imageFormat = 0;
    unsigned int m_internalFormat = 0;

   public:
    Texture2D();
    void bind() const;
    unsigned int getImageFormat() const noexcept;
    unsigned int getWidth() const noexcept;
    unsigned int getHeight() const noexcept;
    unsigned int getID() const noexcept;
    void generate(unsigned int width, unsigned int height, bool alpha, unsigned char *data);
};

using Texture2DView = const Texture2D *;
using Texture2DRef = const Texture2D &;
