#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "custom_attributes.hpp"
#include "string_operators.hpp"
#include "texture_2d.hpp"

class TextureManager {
   private:
    std::unordered_map<std::string, std::unique_ptr<Texture2D>, string_hash, string_view_equal> m_textures;
    bool m_isDefaultTextureLoaded = false;

    const Texture2D &_getDefaultTexture();

   public:
    TextureManager() = default;
    ~TextureManager();
    void loadTexture(std::string path, bool alpha, std::string name);
    const Texture2D &getTexture(std::string_view name);
};
