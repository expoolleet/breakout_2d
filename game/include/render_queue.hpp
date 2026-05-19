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
    Shader *shader;
    Texture2D *texture;
    glm::vec2 position;
    glm::vec2 size;
    float rotation;
    glm::vec4 color;
    RenderLayer layer;
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
