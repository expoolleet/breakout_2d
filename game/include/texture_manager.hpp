#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "custom_attributes.hpp"
#include "path_manager.hpp"
#include "string_operators.hpp"
#include "texture_2d.hpp"

class TextureManager {
   private:
    std::unordered_map<std::string, std::unique_ptr<Texture2D>, string_hash, string_view_equal> m_textures;
    bool m_isDefaultTextureLoaded = false;
    PathManagerPtr m_pathManager;

    Texture2DPtr _getDefaultTexture();

   public:
    TextureManager(PathManagerPtr pathManager);
    ~TextureManager();
    void loadTexture(const std::string &path, bool alpha, std::string_view name);
    Texture2DPtr getTexture(std::string_view name);
};

using TextureManagerPtr = std::unique_ptr<TextureManager>;
