#include "app.hpp"
#include "render.hpp"

int main() {
    render::type = RenderType::OpenGL;

    render::initWindow();
    render::initAPI();
    render::setViewport();

    App app;
    app.run();

    render::terminate();
    return 0;
}
