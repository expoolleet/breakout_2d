#include "text_renderer.hpp"

#include <freetype/freetype.h>
#include <stb_image.h>

#include <algorithm>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string_view>
#include <vector>

#include "game_core.hpp"
#include "glad/glad.h"
#include "logging.hpp"
#include "shader.hpp"
#include "window.hpp"

TextRenderer::TextRenderer(const char *pathToFonts) : m_pathToFonts(pathToFonts) {}

void TextRenderer::_renderText(const std::vector<TextVertex> &vertices) {
    if (vertices.empty()) return;

    glBindTextureUnit(0, m_atlasData.textureID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(TextVertex), vertices.data());

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, int(vertices.size()));
}

glm::mat4 &TextRenderer::_getProjectionMat() noexcept {
    if (updateProjectionMat) {
        updateProjectionMat = false;
        // float halfWidth = core::getWorldWidth() / 2.0f;
        // float halfHeight = core::getWorldHeight() / 2.0f;
        // m_projectionMat = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
        m_projectionMat = glm::ortho(0.0f, static_cast<float>(Window::getWidth()), 0.0f, static_cast<float>(Window::getHeight()));
    }
    return m_projectionMat;
}

TextRenderer::~TextRenderer() {
    FT_Done_FreeType(m_ft);
}

void TextRenderer::initRenderer() {
    if (m_isInitialized) return;
    if (FT_Init_FreeType(&m_ft)) logging::Error("Could not init FreeType Library");

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_maxBufferVertices * TEXT_VERTIX_COUNT_PER_CHAR * TEXT_VERTIX_DATA_PER_CHAR * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_isInitialized = true;
}

void TextRenderer::initFont(std::string_view font, unsigned int fontSize) {
    FT_Face face;
    if (FT_New_Face(m_ft, (m_pathToFonts / font).string().c_str(), 0, &face)) logging::Error("Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    m_atlasData.fontSize = fontSize;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    logging::Log("Initializing font: {}", font);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_atlasData.textureID);
    glTextureStorage2D(m_atlasData.textureID, 1, GL_R8, m_atlasData.width, m_atlasData.height);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    unsigned int xCurrent = 0;
    unsigned int yCurrent = 0;
    unsigned int padding = 2;
    unsigned int maxRowHeight = 0;
    for (unsigned char c = ASCII_TABLE_START; c <= ASCII_TABLE_END; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            logging::Error("Failed to load Glyph: {}", (char)c);
            continue;
        }
        unsigned int w = face->glyph->bitmap.width;
        unsigned int h = face->glyph->bitmap.rows;

        if (xCurrent + w + padding > m_atlasData.width) {
            xCurrent = 0;
            yCurrent += maxRowHeight + padding;
            maxRowHeight = 0;
        }
        if (w > 0 && h > 0) {
            glTextureSubImage2D(m_atlasData.textureID, 0, xCurrent, yCurrent, w, h, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        }

        Character character{glm::ivec2(xCurrent, yCurrent), glm::ivec2(w, h), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                            static_cast<unsigned int>(face->glyph->advance.x)};
        m_characters.insert({c, character});

        xCurrent += w + padding;
        maxRowHeight = std::max(maxRowHeight, h);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    logging::Log("Initializing of font is done");
    FT_Done_Face(face);
}

void TextRenderer::initFontMSDF(std::string_view atlasPath, std::string_view jsonPath) {
    std::ifstream f(jsonPath.data());
    if (!f.is_open()) {
        logging::Error("Could not open the JSON file: {}", jsonPath);
        return;
    }
    json j = json::parse(f);

    try {
        m_atlasData.fontSize = j["atlas"]["size"];
        m_atlasData.distanceRange = j["atlas"]["distanceRange"];

        m_fontMetrics.ascender = j["metrics"]["ascender"];
        m_fontMetrics.descender = j["metrics"]["descender"];
        m_fontMetrics.lineHeight = j["metrics"]["lineHeight"];

        for (auto &item : j["glyphs"]) {
            GlyphData glyphData;
            glyphData.advance = item["advance"];
            glyphData.unicode = item["unicode"];
            if (item.contains("atlasBounds") && item.contains("planeBounds")) {
                glyphData.hasBounds = true;

                glyphData.atlasBounds.left = item["atlasBounds"]["left"];
                glyphData.atlasBounds.top = item["atlasBounds"]["top"];
                glyphData.atlasBounds.right = item["atlasBounds"]["right"];
                glyphData.atlasBounds.bottom = item["atlasBounds"]["bottom"];

                glyphData.planeBounds.left = item["planeBounds"]["left"];
                glyphData.planeBounds.top = item["planeBounds"]["top"];
                glyphData.planeBounds.right = item["planeBounds"]["right"];
                glyphData.planeBounds.bottom = item["planeBounds"]["bottom"];
            }

            m_glyphs[glyphData.unicode] = glyphData;
        }
    } catch (json::exception e) {
        logging::Error("{}", e.what());
        return;
    }

    int width, height, components;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(atlasPath.data(), &width, &height, &components, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glCreateTextures(GL_TEXTURE_2D, 1, &m_atlasData.textureID);
    if (components == 1) {
        glTextureStorage2D(m_atlasData.textureID, 1, GL_R8, width, height);
        glTextureSubImage2D(m_atlasData.textureID, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
    } else if (components == 3) {
        glTextureStorage2D(m_atlasData.textureID, 1, GL_RGB8, width, height);
        glTextureSubImage2D(m_atlasData.textureID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (components == 4) {
        glTextureStorage2D(m_atlasData.textureID, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(m_atlasData.textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void TextRenderer::render(Shader &shader, std::string_view text, glm::vec2 pos, float scale, glm::vec3 color) {
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size() * 6);

    float advance = 0.0f;
    for (const char &c : text) {
        Character character = m_characters[c];

        float xPos = pos.x + advance + character.bearing.x * scale;
        float yPos = pos.y - (character.size.y - character.bearing.y) * scale;

        float uStart = static_cast<float>(character.position.x) / m_atlasData.width;
        float vStart = static_cast<float>(character.position.y) / m_atlasData.height;
        float uEnd = static_cast<float>(character.position.x + character.size.x) / m_atlasData.width;
        float vEnd = static_cast<float>(character.position.y + character.size.y) / m_atlasData.height;

        advance += (character.advance >> 6) * scale;

        float w = character.size.x * scale;
        float h = character.size.y * scale;

        vertices.push_back({xPos, yPos + h, uStart, vStart});
        vertices.push_back({xPos, yPos, uStart, vEnd});
        vertices.push_back({xPos + w, yPos, uEnd, vEnd});
        vertices.push_back({xPos, yPos + h, uStart, vStart});
        vertices.push_back({xPos + w, yPos, uEnd, vEnd});
        vertices.push_back({xPos + w, yPos + h, uEnd, vStart});
    }
    shader.use();
    shader.setVec3("textColor", color);
    shader.setMat4("projection", _getProjectionMat());  // move this to better place
    _renderText(vertices);
}

void TextRenderer::renderMSDF(Shader &shader, std::string_view text, glm::vec2 pos, float scale, glm::vec3 color, bool bold,
                              OutlineData outlineData) {
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size() * 6);

    int atlasW, atlasH;
    glGetTextureLevelParameteriv(m_atlasData.textureID, 0, GL_TEXTURE_WIDTH, &atlasW);
    glGetTextureLevelParameteriv(m_atlasData.textureID, 0, GL_TEXTURE_HEIGHT, &atlasH);

    float currentFontSize = m_atlasData.fontSize * scale;

    for (const char &c : text) {
        if (!m_glyphs.contains(static_cast<int>(c))) continue;
        GlyphData &glyph = m_glyphs[static_cast<int>(c)];

        if (glyph.hasBounds) {
            float xPos0 = pos.x + glyph.planeBounds.left * currentFontSize;
            float yPos0 = pos.y + glyph.planeBounds.bottom * currentFontSize;
            float xPos1 = pos.x + glyph.planeBounds.right * currentFontSize;
            float yPos1 = pos.y + glyph.planeBounds.top * currentFontSize;

            float u0 = glyph.atlasBounds.left / static_cast<float>(atlasW);
            float v0 = glyph.atlasBounds.bottom / static_cast<float>(atlasH);
            float u1 = glyph.atlasBounds.right / static_cast<float>(atlasW);
            float v1 = glyph.atlasBounds.top / static_cast<float>(atlasH);

            vertices.push_back({xPos0, yPos1, u0, v1});
            vertices.push_back({xPos0, yPos0, u0, v0});
            vertices.push_back({xPos1, yPos0, u1, v0});

            vertices.push_back({xPos0, yPos1, u0, v1});
            vertices.push_back({xPos1, yPos0, u1, v0});
            vertices.push_back({xPos1, yPos1, u1, v1});
        }
        pos.x += glyph.advance * currentFontSize;
    }
    shader.setVec3("textColor", color);
    shader.setMat4("projection", _getProjectionMat());
    shader.setFloat("distanceRange", m_atlasData.distanceRange);
    shader.setFloat("outlineWidth", outlineData.width);
    shader.setVec3("outlineColor", outlineData.color);
    shader.setBool("isBold", bold);
    _renderText(vertices);
}

void TextRenderer::setCharLimit(unsigned int limit) {
    m_maxBufferVertices = limit;
}
