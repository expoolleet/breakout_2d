#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "custom_attributes.hpp"
#include "string_operators.hpp"
#include "texture_2d.hpp"

#define DEFAULT_TEXTURE_NAME "default"
#define STANDARD_BRICK "standard_brick"
#define HARD_BRICK "hard_brick"
#define EXTREMELY_TOUGH_BRICK "extremely_tough_brick"
#define UNDESTROYABLE_BRICK "undestroyable_brick"

class TextureManager {
   private:
    NO_DESTROY_ATTR inline static std::unordered_map<std::string, std::unique_ptr<Texture2D>, string_hash, string_view_equal> m_textures =
        {};
    inline static bool m_isDefaultTextureLoaded = false;
    TextureManager operator=(const TextureManager &) = delete;
    TextureManager(const TextureManager &) = delete;
    TextureManager() = delete;
    static const Texture2D &_getDefaultTexture();

   public:
    ~TextureManager();
    static void loadTexture(std::string path, bool alpha, std::string name);
    static const Texture2D &getTexture(std::string_view name);
};
