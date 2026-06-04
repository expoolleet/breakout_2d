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

inline constexpr std::string_view ARIAL_FONT = "arial.ttf";
inline constexpr std::string_view ROBOTO_LIGHT_FONT = "roboto-light.ttf";
inline constexpr std::string_view ROBOTO_MEDIUM_FONT = "roboto-medium.ttf";

inline constexpr int TEXT_VERTIX_COUNT_PER_CHAR = 6;
inline constexpr int TEXT_VERTIX_DATA_PER_CHAR = 4;

inline constexpr int ASCII_TABLE_START = 32;
inline constexpr int ASCII_TABLE_END = 126;

inline constexpr float MONO_SPACE_SCALE = 0.6f;

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
    float left;
    float top;
    float right;
    float bottom;
};

struct GlyphInfo {
    float advance;
    Bounds atlasBounds;
    Bounds planeBounds;
    int unicode;
    bool hasBounds;
};

struct FontMetrics {
    float ascender;
    float descender;
    float emSize;
    float lineHeight;
    float underlineThickness;
    float underlineY;
};

struct AtlasInfo {
    unsigned int textureID;
    unsigned int fontSize;
    unsigned int width;
    unsigned int height;
    unsigned int distanceRange;
};

enum class TextAlign {
    Left,
    Center,
    Right,
};

struct TextInfo {
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float scale = 1.0f;
    TextAlign align = TextAlign::Left;
    bool isBold = false;
};

struct OutlineData {
    glm::vec3 color = glm::vec3(0.0f);
    float width = 0.0f;
};

class TextRenderer {
   public:
    TextRenderer(const std::string &fontsPath);
    ~TextRenderer();

    bool updateProjectionMat = true;

    void initRenderer();
    void initFont(std::string_view font, unsigned int fontSize);
    void initFontMSDF(std::string_view atlasPath, std::string_view jsonPath);
    void render(Shader &shader, std::string_view text, const TextInfo &textData);
    void renderMSDF(Shader &shader, std::string_view text, const TextInfo &textData, const OutlineData &outlineData = {});
    void setCharLimit(unsigned int limit);

   private:
    glm::mat4 m_projectionMat;
    std::unordered_map<char, Character> m_characters;
    std::unordered_map<unsigned char, GlyphInfo> m_glyphs;
    FontMetrics m_fontMetrics;
    AtlasInfo m_atlasData;
    FT_Library m_ft = nullptr;
    fs::path m_fontsPath;
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_maxBufferVertices = 500;
    bool m_isInitialized = false;

    void _renderText(const std::vector<TextVertex> &vertices) const;
    glm::mat4 &_getProjectionMat() noexcept;
};

using TextRendererPtr = observer_ptr<TextRenderer>;
