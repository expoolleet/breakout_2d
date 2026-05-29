#pragma once

#include <freetype/config/ftheader.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string_view>
#include <unordered_map>
#include <vector>
#include FT_FREETYPE_H

#include "shader.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

#define ARIAL_FONT "arial.ttf"
#define ROBOTO_LIGHT_FONT "roboto-light.ttf"
#define ROBOTO_MEDIUM_FONT "roboto-medium.ttf"

#define TEXT_VERTIX_COUNT_PER_CHAR 6
#define TEXT_VERTIX_DATA_PER_CHAR 4

#define ASCII_TABLE_START 32
#define ASCII_TABLE_END 126

struct Character {
    glm::ivec2 position;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

struct TextVertex {
    float x;
    float y;
    float u;
    float v;
};

struct Bounds {
    float left = 0.0;
    float top = 0.0;
    float right = 0.0;
    float bottom = 0.0;
};

struct GlyphData {
    float advance;
    Bounds atlasBounds;
    Bounds planeBounds;
    int unicode;
    bool hasBounds = false;
};

struct FontMetrics {
    float ascender;
    float descender;
    float emSize;
    float lineHeight;
    float underlineThickness;
    float underlineY;
};

struct AtlasData {
    unsigned int textureID;
    unsigned int fontSize;
    unsigned int width;
    unsigned int height;
    unsigned int distanceRange = 0;
};

struct OutlineData {
    glm::vec3 color = glm::vec3(1.0f);
    float width = 0.0f;
};

class TextRenderer {
   private:
    fs::path m_fontsPath;
    std::unordered_map<char, Character> m_characters;
    std::unordered_map<unsigned char, GlyphData> m_glyphs;
    FontMetrics m_fontMetrics;
    AtlasData m_atlasData;
    unsigned int m_maxBufferVertices = 500;
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    bool m_isInitialized = false;
    FT_Library m_ft = nullptr;
    glm::mat4 m_projectionMat;

    void _renderText(const std::vector<TextVertex> &vertices);
    glm::mat4 &_getProjectionMat() noexcept;

   public:
    TextRenderer(const std::string &fontsPath);
    ~TextRenderer();

    bool updateProjectionMat = true;

    void initRenderer();
    void initFont(std::string_view font, unsigned int fontSize);
    void initFontMSDF(std::string_view atlasPath, std::string_view jsonPath);
    void render(Shader &shader, std::string_view text, glm::vec2 pos, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f));
    void renderMSDF(Shader &shader, std::string_view text, glm::vec2 pos, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f),
                    bool bold = false, OutlineData outlineData = {});
    void setCharLimit(unsigned int limit);
};

using TextRendererPtr = observer_ptr<TextRenderer>;
