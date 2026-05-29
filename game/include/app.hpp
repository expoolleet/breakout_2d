#pragma once
#include "canvas.hpp"
#include "engine_context.hpp"
#include "object_manager.hpp"
#include "shader_observer.hpp"

class App {
   public:
    Context context;
    ObjectManager objectManager;
    ShaderObserver shaderObserver;
    Canvas canvas;

    App();
    void run();
};
