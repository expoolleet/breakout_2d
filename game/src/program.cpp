#include "audio_manager.hpp"
#include "canvas.hpp"
#include "custom_attributes.hpp"
#include "game.hpp"
#include "game_core.hpp"
#include "path_manager.hpp"
#include "render.hpp"
#include "render_type.hpp"
#include "shader_observer.hpp"
#include "window.hpp"

#define FIXED_FRAMETIME (1.0 / 100.0)  // 100 Hz fixed update loop
#define MAX_FRAMETIME 0.25

#define MSAA_SAMPLES 4

static bool enableWireframe = false;
NO_DESTROY_ATTR static BufferObject resolveBuffer;
NO_DESTROY_ATTR static BufferObject msaaBuffer;
NO_DESTROY_ATTR static Game game(3);

void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        game.Keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        game.Keys[key] = false;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        enableWireframe = !enableWireframe;
        if (enableWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        game.spawnBall(glm::vec2(0.0f));
    }
}

void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    if (width < 1 || height < 1) return;
    Window::setWidth(width);
    Window::setHeight(height);
    glViewport(0, 0, width, height);
    game.setProjectionMatrix();
    resolveBuffer = render::resizeFrambuffer(resolveBuffer);
    msaaBuffer = render::resizeMultisamplingFrambuffer(msaaBuffer);
}

int main() {
    render::renderType = RenderType::OpenGL;

    render::initWindow();
    render::initAPI();
    render::setFrameBufferSizeCallback(framebufferResizeCallback);
    render::setKeyCallback(keyCallback);

    render::configureViewport();

    render::enableVSync(true);

    render::setupDefaultAlphaBlending();
    render::setupMultisampling();

    Canvas::init();
    Canvas canvas;
    resolveBuffer = render::getFramebuffer(1);
    msaaBuffer = render::getMultisamlpingFramebuffer(MSAA_SAMPLES);

    std::string shaders = PathManager::getResourcePath("shaders");
    Shader canvasShader(shaders + "/vs/canvas.glsl", shaders + "/fs/canvas.glsl");
    canvasShader.setVec2("inverseScreenSize",
                         glm::vec2(1.0f / static_cast<float>(Window::getWidth()), 1.0f / static_cast<float>(Window::getHeight())));

    PathManager::init();
    ShaderObserver::Get().startObserving();
    game.init();

    AudioManager::Get().init();
    AudioManager::Get().loadBank("master");

    double frameTime = 0.0;
    double lastTime = 0.0;
    double accumulation = 0.0;
    while (!render::windowShouldClose()) {
        double currentTime = glfwGetTime();
        frameTime = currentTime - lastTime;
        lastTime = currentTime;
        if (frameTime > MAX_FRAMETIME) frameTime = MAX_FRAMETIME;
        accumulation += frameTime;
        render::PollWindowEvents();
        game.processInput(static_cast<float>(frameTime));
        while (accumulation >= FIXED_FRAMETIME) {
            game.fixedUpdate(static_cast<float>(FIXED_FRAMETIME));
            accumulation -= FIXED_FRAMETIME;
        }
        game.update(static_cast<float>(frameTime));

        glBindFramebuffer(GL_FRAMEBUFFER, msaaBuffer.fbo);
        glClear(GL_COLOR_BUFFER_BIT);
        float alpha = static_cast<float>(accumulation / FIXED_FRAMETIME);
        game.render(alpha);
        glBlitNamedFramebuffer(msaaBuffer.fbo, resolveBuffer.fbo, 0, 0, Window::getWidth(), Window::getHeight(), 0, 0, Window::getWidth(),
                               Window::getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

        AudioManager::Get().update();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        canvasShader.use();
        canvas.render(canvasShader, resolveBuffer.attachments);
        game.renderText(alpha);
        render::swapBuffers();
        ShaderObserver::Get().update();
    }
    ShaderObserver::Get().stopObserving();
    glfwTerminate();
    return 0;
}
