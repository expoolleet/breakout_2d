#include "texture_manager.hpp"

#include "logging.hpp"
#include "texture_2d.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "texture_literals.hpp"

using namespace texture_literals;

Texture2DPtr TextureManager::_getDefaultTexture() {
    if (!m_isDefaultTextureLoaded) {
        m_isDefaultTextureLoaded = true;
        loadTexture(m_pathManager->getResourcePath("textures/missing_texture.jpg"), false, DEFAULT_TEXTURE);
    }
    return m_textures[DEFAULT_TEXTURE.data()].get();
}

TextureManager::TextureManager(PathManagerPtr pathManager) : m_pathManager(pathManager) {}

TextureManager::~TextureManager() {
    m_textures.clear();
}

void TextureManager::loadTexture(const std::string &path, bool alpha, std::string_view name) {
    if (m_textures.contains(name)) {
        logging::Warn("Texture with name {} is already loaded, check if you wrong", name);
        return;
    }

    int width, height, componentCount;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.data(), &width, &height, &componentCount, 0);
    auto texture = std::make_unique<Texture2D>();
    if (data) {
        texture->generate(width, height, alpha, data);
    } else {
        logging::Error("Could not load texture by path: {}", path);
        return;
    }
    m_textures[name.data()] = std::move(texture);
    stbi_image_free(data);
}

Texture2DPtr TextureManager::getTexture(std::string_view name) {
    auto it = m_textures.find(name);
    if (it == m_textures.end()) {
        logging::Error("No texture with name {} was loaded", name);
        return _getDefaultTexture();
    }
    return it->second.get();
}
