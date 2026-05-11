#include "audio_manager.hpp"
#include "canvas.hpp"
#include "custom_attributes.hpp"
#include "engine_context.hpp"
#include "game.hpp"
#include "game_core.hpp"
#include "render.hpp"
#include "render_type.hpp"
#include "shader_observer.hpp"
#include "window.hpp"

constexpr float FIXED_FRAMETIME = 1.0f / 100.0f;  // 100 Hz fixed update loop
constexpr float MAX_FRAMETIME = 0.25f;
constexpr int MSAA_SAMPLES = 4;

NO_DESTROY_ATTR static BufferObject resolveBuffer;
NO_DESTROY_ATTR static BufferObject msaaBuffer;
NO_DESTROY_ATTR static Game game{3};

void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        game.keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        game.keys[key] = false;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        game.spawnBall(glm::vec2(0.0f));
    }
}

void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    if (width < 1 || height < 1) return;
    Window::setWidth(width);
    Window::setHeight(height);
    render::setViewport();
    game.setProjectionMatrix();
    resolveBuffer = render::resizeFrambuffer(resolveBuffer);
    msaaBuffer = render::resizeMultisamplingFrambuffer(msaaBuffer);
}

int main() {
    Context &context = Context::get();

    render::renderType = RenderType::OpenGL;

    render::initWindow();
    render::initAPI();
    render::setFrameBufferSizeCallback(framebufferResizeCallback);
    render::setKeyCallback(keyCallback);

    render::setViewport();

    render::enableVSync(true);

    render::setupDefaultAlphaBlending();
    render::setupMultisampling();

    Canvas::init();
    Canvas canvas;
    resolveBuffer = render::getFramebuffer(1);
    msaaBuffer = render::getMultisamlpingFramebuffer(MSAA_SAMPLES);

    std::string shaders = context.pathManager->getResourcePath("shaders");
    Shader canvasShader(shaders + "/vs/canvas.glsl", shaders + "/fs/canvas.glsl");
    canvasShader.setVec2("inverseScreenSize",
                         glm::vec2(1.0f / static_cast<float>(Window::getWidth()), 1.0f / static_cast<float>(Window::getHeight())));
    game.init();

    context.audioManager->loadBank("master");

    float frameTime = 0.0;
    float lastTime = 0.0;
    float accumulation = 0.0;
    while (!render::windowShouldClose()) {
        float currentTime = render::getTime();
        frameTime = currentTime - lastTime;
        lastTime = currentTime;
        frameTime = std::fmin(frameTime, MAX_FRAMETIME);
        accumulation += frameTime;
        while (accumulation >= FIXED_FRAMETIME) {
            game.fixedUpdate(FIXED_FRAMETIME);
            accumulation -= FIXED_FRAMETIME;
        }
        game.update(frameTime);

        render::bindFramebuffer(msaaBuffer.fbo);
        float alpha = accumulation / FIXED_FRAMETIME;
        game.render(alpha);
        render::blitFramebuffer(msaaBuffer.fbo, resolveBuffer.fbo);
        context.audioManager->update();

        render::bindFramebuffer(0);
        canvasShader.use();
        canvas.render(canvasShader, resolveBuffer.attachments);
        game.renderText(alpha);

        render::pollWindowEvents();
        game.processInput(frameTime);

        render::swapBuffers();
        context.shaderObserver->update();
    }
    render::terminate();
    return 0;
}
