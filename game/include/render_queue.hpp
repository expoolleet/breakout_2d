#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "shader.hpp"
#include "texture_2d.hpp"

enum class RenderLayer {
    Background = 0,
    Brick = 10,
    Player = 20,
    Particle = 30,
    Ball = 40,
    PowerUp = 50,
    UI = 60,
};

struct SpriteCommand {
    RenderLayer layer;
    Shader *shader;
    const Texture2D *texture;
    glm::vec2 position;
    glm::vec2 size;
    float rotation;
    glm::vec4 color;

    SpriteCommand(RenderLayer l, Shader *sh, const Texture2D *tex)
        : layer(l), shader(sh), texture(tex), position(glm::vec2(0.0f)), size(glm::vec2(1.0f)), rotation(0.0f), color(glm::vec4(1.0f)) {}

    SpriteCommand(RenderLayer l, Shader *sh, const Texture2D *tex, glm::vec2 pos)
        : layer(l), shader(sh), texture(tex), position(pos), size(glm::vec2(1.0f)), rotation(0.0f), color(glm::vec4(1.0f)) {}

    SpriteCommand(RenderLayer l, Shader *sh, const Texture2D *tex, glm::vec2 pos, glm::vec2 s)
        : layer(l), shader(sh), texture(tex), position(pos), size(s), rotation(0.0f), color(glm::vec4(1.0f)) {}

    SpriteCommand(RenderLayer l, Shader *sh, const Texture2D *tex, glm::vec2 pos, glm::vec2 s, float r)
        : layer(l), shader(sh), texture(tex), position(pos), size(s), rotation(r), color(glm::vec4(1.0f)) {}

    SpriteCommand(RenderLayer l, Shader *sh, const Texture2D *tex, glm::vec2 pos, glm::vec2 s, float r, glm::vec4 col)
        : layer(l), shader(sh), texture(tex), position(pos), size(s), rotation(r), color(col) {}
};

using SpriteCommands = std::vector<SpriteCommand>;

class RenderQueue {
   public:
    void push(const SpriteCommand &command);
    void sort();
    void clear();

    const SpriteCommands &getCommands() const;

   private:
    std::vector<SpriteCommand> m_commands;
};
