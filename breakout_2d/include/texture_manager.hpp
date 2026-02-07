#pragma once

#include "string_operators.hpp"
#include "texture_2d.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

#define DEFAULT_TEXTURE_NAME "default"
#define STANDARD_BRICK "standard_brick"
#define HARD_BRICK "hard_brick"
#define EXTREMELY_TOUGH_BRICK "extremely_tough_brick"
#define UNDESTROYABLE_BRICK "undestroyable_brick"

class TextureManager {
private:
	TextureManager() = delete;
	TextureManager(TextureManager &) = delete;
	TextureManager operator=(TextureManager &) = delete;
	inline static std::unordered_map<std::string, std::unique_ptr<Texture2D>, string_hash, string_view_equal> m_textures = { };
	inline static bool m_isDefaultTextureLoaded = false;
	static const Texture2D &_getDefaultTexture();
public:
	~TextureManager();
	static void loadTexture(std::string path, bool alpha, std::string name);
	static const Texture2D& getTexture(std::string_view name);
};