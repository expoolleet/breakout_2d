#pragma once

#include <glm/glm.hpp>

#include "object_2d.hpp"

class Background : public Object2D {
   public:
    Background(ContextPtr context, Texture2DPtr texture);
};
