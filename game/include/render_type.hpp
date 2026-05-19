#pragma once

enum class RenderType {
    Unknown,
    OpenGL,
    Vulkan,
};

namespace render {
extern RenderType type;
}
