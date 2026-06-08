#pragma once

#include <glm/glm.hpp>

#include "sprite_object.hpp"

class Background : public SpriteObject {
   public:
    Background(ContextPtr context, Texture2DPtr texture);
};
