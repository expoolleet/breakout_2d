#include "app.hpp"
#include "render.hpp"

int main() {
    render::type = RenderType::OpenGL;

    render::initWindow();
    render::initAPI();
    render::setViewport();

    App app;
    app.run();
    app.close();
    return EXIT_SUCCESS;
}
