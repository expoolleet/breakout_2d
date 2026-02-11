#include "logging.hpp"
#include "path_manager.hpp"
#include "texture_2d.hpp"
#include "texture_manager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <string>
#include <format>
#include <string_view>
#include <utility>
#include <memory>

const Texture2D &TextureManager::_getDefaultTexture() {
	if (!m_isDefaultTextureLoaded) {
		m_isDefaultTextureLoaded = true;
		loadTexture(PathManager::getResourcePath("textures/missing_texture.jpg"), false, DEFAULT_TEXTURE_NAME);
	}
	return *(m_textures[DEFAULT_TEXTURE_NAME]);
}

TextureManager::~TextureManager() {
	m_textures.clear();
}

void TextureManager::loadTexture(std::string path, bool alpha, std::string name) {
	if (m_textures.contains(name)) {
		Logging::Warn(std::format("Texture with name {} is already loaded, check if you wrong", name));
		return;
	}

	int width, height, componentCount;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.data(), &width, &height, &componentCount, 0);
	unsigned int textureID = 0;
	auto texture = std::make_unique<Texture2D>();
	if (data) {
		texture->generate(width, height, alpha, data);
	}
	else {
		Logging::Error(std::format("Could not load texture by path: {}", path));
		return;
	}
	m_textures[name] = std::move(texture);
	stbi_image_free(data);
}

const Texture2D& TextureManager::getTexture(std::string_view name) {
	auto it = m_textures.find(name);
	if (it == m_textures.end()) {
		Logging::Error(std::format("No texture with name {} was loaded", name));
		return _getDefaultTexture();
	}
	return *(it->second);
}
