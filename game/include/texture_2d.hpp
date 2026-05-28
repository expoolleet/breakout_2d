#pragma once

#include "observer_ptr.hpp"
class Texture2D {
   private:
    unsigned int m_ID = 0;
    unsigned int m_width = 0;
    unsigned int m_height = 0;
    unsigned int m_imageFormat = 0;
    unsigned int m_internalFormat = 0;

   public:
    Texture2D() = default;

    bool operator==(const Texture2D &other) const noexcept;

    void bind() const;
    unsigned int getImageFormat() const noexcept;
    unsigned int getWidth() const noexcept;
    unsigned int getHeight() const noexcept;
    unsigned int getID() const noexcept;
    void generate(unsigned int width, unsigned int height, bool alpha, unsigned char *data);
};

using Texture2DPtr = observer_ptr<Texture2D>;
