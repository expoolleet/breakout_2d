#pragma once

#include "glad/glad.h"
#include "render_type.hpp"

struct RenderConfig {
    RenderType type;
    unsigned int msaaSamples;
};
