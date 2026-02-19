#pragma once

#include <vector>

class Shader; // fwd

class Canvas {
  private:
    inline static bool m_initialized = false;
    inline static unsigned int m_VAO = 0;

  public:
    Canvas() = default;
    static void init();
    void render(Shader &shader, std::vector<unsigned int> &colorAttachments);
};
