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
#include "render_type.hpp"
#include "shader.hpp"

TextRenderer::TextRenderer(const std::string &fontsPath) : m_fontsPath(fontsPath) {}

TextRenderer::~TextRenderer() {
    FT_Done_FreeType(m_ft);
}

void TextRenderer::initRenderer() {
    if (m_isInitialized) return;
    m_isInitialized = true;
    if (FT_Init_FreeType(&m_ft)) {
        logging::Error("Could not init FreeType Library");
        return;
    }

    if (render::type == RenderType::OpenGL) {
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
    }
}

void TextRenderer::initFont(std::string_view font, unsigned int fontSize) {
    logging::Info("Initializing font: {}", font);

    FT_Face face;
    if (FT_New_Face(m_ft, (m_fontsPath / font).string().c_str(), 0, &face)) {
        logging::Error("Failed to load font");
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    m_atlasData.fontSize = fontSize;

    if (render::type == RenderType::OpenGL) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glCreateTextures(GL_TEXTURE_2D, 1, &m_atlasData.textureID);
        glTextureStorage2D(m_atlasData.textureID, 1, GL_R8, m_atlasData.width, m_atlasData.height);
        glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_atlasData.textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

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
            if (render::type == RenderType::OpenGL) {
                glTextureSubImage2D(m_atlasData.textureID, 0, xCurrent, yCurrent, w, h, GL_RED, GL_UNSIGNED_BYTE,
                                    face->glyph->bitmap.buffer);
            }
        }

        Character character{glm::ivec2(xCurrent, yCurrent), glm::ivec2(w, h), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                            static_cast<unsigned int>(face->glyph->advance.x)};
        m_characters.insert({c, character});

        xCurrent += w + padding;
        maxRowHeight = std::max(maxRowHeight, h);
    }
    if (render::type == RenderType::OpenGL) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
    logging::Info("Initializing of font is done");
    FT_Done_Face(face);
}

void TextRenderer::initFontMSDF(std::string_view atlasPath, std::string_view jsonPath) {
    std::ifstream f(jsonPath.data());
    if (!f.is_open()) {
        logging::Error("Could not open the JSON file: {}", jsonPath);
        return;
    }
    json fontData = json::parse(f);

    try {
        m_atlasData.fontSize = fontData["atlas"]["size"];
        m_atlasData.distanceRange = fontData["atlas"]["distanceRange"];

        m_fontMetrics.ascender = fontData["metrics"]["ascender"];
        m_fontMetrics.descender = fontData["metrics"]["descender"];
        m_fontMetrics.lineHeight = fontData["metrics"]["lineHeight"];

        for (auto &item : fontData["glyphs"]) {
            GlyphInfo glyphData;
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

            m_glyphs[static_cast<unsigned char>(glyphData.unicode)] = glyphData;
        }
    } catch (json::exception e) {
        logging::Error("{}", e.what());
        return;
    }

    int width, height, components;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(atlasPath.data(), &width, &height, &components, 0);
    if (render::type == RenderType::OpenGL) {
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

    free(data);
}

void TextRenderer::render(Shader &shader, std::string_view text, const TextInfo &textData) {
    if (text.empty()) {
        logging::Error("(Text Renderer) Text is empty");
        return;
    }
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size() * 6);

    float advance = 0.0f;

    glm::vec2 currentPos = textData.position;
    std::stringstream input(text.data());
    for (std::string line; std::getline(input, line);) {
        for (const char &c : text) {
            Character character = m_characters[c];

            float xPos = currentPos.x + advance + static_cast<float>(character.bearing.x) * textData.scale;
            float yPos = currentPos.y - (static_cast<float>(character.size.y) - static_cast<float>(character.bearing.y)) * textData.scale;

            float uStart = static_cast<float>(character.position.x) / static_cast<float>(m_atlasData.width);
            float vStart = static_cast<float>(character.position.y) / static_cast<float>(m_atlasData.height);
            float uEnd = static_cast<float>(character.position.x + character.size.x) / static_cast<float>(m_atlasData.width);
            float vEnd = static_cast<float>(character.position.y + character.size.y) / static_cast<float>(m_atlasData.height);

            advance += static_cast<float>(character.advance >> 6) * textData.scale;

            float w = static_cast<float>(character.size.x) * textData.scale;
            float h = static_cast<float>(character.size.y) * textData.scale;

            vertices.emplace_back(xPos, yPos + h, uStart, vStart);
            vertices.emplace_back(xPos, yPos, uStart, vEnd);
            vertices.emplace_back(xPos + w, yPos, uEnd, vEnd);
            vertices.emplace_back(xPos, yPos + h, uStart, vStart);
            vertices.emplace_back(xPos + w, yPos, uEnd, vEnd);
            vertices.emplace_back(xPos + w, yPos + h, uEnd, vStart);
        }
    }
    shader.use();
    shader.setVec3("textColor", textData.color);
    shader.setMat4("projection", _getProjectionMat());  // move this to better place
    _renderText(vertices);
}

void TextRenderer::renderMSDF(Shader &shader, std::string_view text, const TextInfo &textInfo, const OutlineData &outlineData) {
    if (text.empty()) {
        logging::Error("(Text Renderer) Text is empty");
        return;
    }
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size() * 6);

    int atlasW = 0, atlasH = 0;
    if (render::type == RenderType::OpenGL) {
        glGetTextureLevelParameteriv(m_atlasData.textureID, 0, GL_TEXTURE_WIDTH, &atlasW);
        glGetTextureLevelParameteriv(m_atlasData.textureID, 0, GL_TEXTURE_HEIGHT, &atlasH);
    }

    if (atlasW == 0 || atlasH == 0) {
        logging::Error("Atlas size is wrong: w:{}; h:{}", atlasW, atlasH);
        return;
    }

    float scaledFontSize = static_cast<float>(m_atlasData.fontSize) * textInfo.scale / core::getWorldWidth();

    glm::vec2 initPos = textInfo.position;
    glm::vec2 currentPos = initPos;

    size_t maxLength = 0.0f;
    std::vector<std::string> lines;
    std::stringstream input(text.data());
    for (std::string line; std::getline(input, line);) {
        lines.push_back(line);
        if (maxLength < line.size()) {
            maxLength = line.size();
        }
    }
    for (const auto &line : lines) {
        if (textInfo.align == TextAlign::Right) {
            currentPos.x += static_cast<float>(maxLength - line.size()) * scaledFontSize * MONO_SPACE_SCALE;
        } else if (textInfo.align == TextAlign::Center) {
            currentPos.x += static_cast<float>(maxLength - line.size()) * scaledFontSize * MONO_SPACE_SCALE / 2.0f;
        }
        for (const char &c : line) {
            if (!m_glyphs.contains(static_cast<unsigned char>(c))) continue;
            GlyphInfo &glyph = m_glyphs[static_cast<unsigned char>(c)];

            if (glyph.hasBounds) {
                float xPos0 = currentPos.x + glyph.planeBounds.left * scaledFontSize;
                float yPos0 = currentPos.y + glyph.planeBounds.bottom * scaledFontSize;
                float xPos1 = currentPos.x + glyph.planeBounds.right * scaledFontSize;
                float yPos1 = currentPos.y + glyph.planeBounds.top * scaledFontSize;

                float u0 = glyph.atlasBounds.left / static_cast<float>(atlasW);
                float v0 = glyph.atlasBounds.bottom / static_cast<float>(atlasH);
                float u1 = glyph.atlasBounds.right / static_cast<float>(atlasW);
                float v1 = glyph.atlasBounds.top / static_cast<float>(atlasH);

                vertices.emplace_back(xPos0, yPos1, u0, v1);
                vertices.emplace_back(xPos0, yPos0, u0, v0);
                vertices.emplace_back(xPos1, yPos0, u1, v0);
                vertices.emplace_back(xPos0, yPos1, u0, v1);
                vertices.emplace_back(xPos1, yPos0, u1, v0);
                vertices.emplace_back(xPos1, yPos1, u1, v1);
            }
            currentPos.x += glyph.advance * scaledFontSize;
        }
        currentPos.y -= m_fontMetrics.lineHeight * scaledFontSize;
        currentPos.x = initPos.x;
    }

    shader.setVec3("textColor", textInfo.color);
    shader.setMat4("projection", _getProjectionMat());
    shader.setFloat("distanceRange", static_cast<float>(m_atlasData.distanceRange));
    shader.setFloat("outlineWidth", outlineData.width);
    shader.setVec3("outlineColor", outlineData.color);
    shader.setBool("isBold", textInfo.isBold);
    _renderText(vertices);
}

void TextRenderer::setCharLimit(unsigned int limit) {
    m_maxBufferVertices = limit;
}

void TextRenderer::_renderText(const std::vector<TextVertex> &vertices) const {
    if (vertices.empty()) {
        logging::Warn("Text Renderer: vertices array is empty");
        return;
    }

    if (render::type == RenderType::OpenGL) {
        glBindTextureUnit(0, m_atlasData.textureID);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(TextVertex), vertices.data());

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, int(vertices.size()));
    }
}

glm::mat4 &TextRenderer::_getProjectionMat() noexcept {
    if (updateProjectionMat) {
        updateProjectionMat = false;
        m_projectionMat = core::getScaledProjectionMatrix();
    }
    return m_projectionMat;
}
