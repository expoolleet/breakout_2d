#include "audio_manager.hpp"
#include "canvas.hpp"
#include "custom_attributes.hpp"
#include "engine_context.hpp"
#include "fast_random.hpp"
#include "fps_counter.hpp"
#include "game.hpp"
#include "object_manager.hpp"
#include "render.hpp"
#include "shader_observer.hpp"
#include "texture_literals.hpp"
#include "timer.hpp"
#include "window.hpp"

using namespace texture_literals;

constexpr float FIXED_FRAMETIME = 1.0f / 100.0f;  // 100 Hz fixed update loop
constexpr float MAX_FRAMETIME = 0.25f;
constexpr int MSAA_SAMPLES = 4;

NO_DESTROY_ATTR static BufferObject resolveBuffer;
NO_DESTROY_ATTR static BufferObject msaaBuffer;

static void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
        game->keys.press(key);
    } else if (action == GLFW_RELEASE) {
        game->keys.unpress(key);
    }
#ifdef _DEBUG
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        game->spawnBall(glm::vec2(0.0f));
    }
#endif
}

static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (width < 1 || height < 1) return;
    Window::setWidth(width);
    Window::setHeight(height);
    render::setViewport();
    game->setProjectionMatrix();
    resolveBuffer = render::resizeFrambuffer(resolveBuffer);
    msaaBuffer = render::resizeMultisamplingFrambuffer(msaaBuffer);
}

int main() {
    render::type = RenderType::OpenGL;

    render::initWindow();
    render::initAPI();

    render::setFrameBufferSizeCallback(framebufferResizeCallback);

    render::setKeyCallback(keyCallback);

    render::setViewport();

    render::enableVSync(true);

    render::setupDefaultAlphaBlending();
    render::setupMultisampling();

    auto context = std::make_unique<Context>();

    auto objectManager = std::make_unique<ObjectManager>(context.get());

    auto shaderObserver = std::make_unique<ShaderObserver>();

    std::string texturesPath = context->pathManager->getResourcePath("textures");
    context->textureManager->loadTexture(texturesPath + "/background_2.png", false, BACKGROUND_TEXTURE);
    context->textureManager->loadTexture(texturesPath + "/brick_standard.png", true, BRICK_TEXTURE);
    context->textureManager->loadTexture(texturesPath + "/brick_undestroyable.png", true, BADROCK_TEXTURE);
    context->textureManager->loadTexture(texturesPath + "/ball.png", true, "ball");
    context->textureManager->loadTexture(texturesPath + "/player_skin_2.png", true, PLAYER_TEXTURE);

    std::string shadersPath = context->pathManager->getResourcePath("shaders");

    auto spriteShader = std::make_unique<Shader>(shadersPath + "/vs/sprite.glsl", shadersPath + "/fs/sprite.glsl", shaderObserver.get());
    spriteShader->setMat4("view", glm::mat4(1.0f));
    spriteShader->setInt("sprite", 0);

    auto particleShader =
        std::make_unique<Shader>(shadersPath + "/vs/particle.glsl", shadersPath + "/fs/particle.glsl", shaderObserver.get());
    particleShader->setMat4("view", glm::mat4(1.0f));
    particleShader->setInt("sprite", 0);

    auto textShader = std::make_unique<Shader>(shadersPath + "/vs/text.glsl", shadersPath + "/fs/textMSDF.glsl", shaderObserver.get());

    std::string fontsPath = context->pathManager->getResourcePath("fonts");
    auto textRenderer = std::make_unique<TextRenderer>(fontsPath);
    textRenderer->initRenderer();
    textRenderer->initFontMSDF(fontsPath + "/msdf/roboto/atlas.png", fontsPath + "/msdf/roboto/atlas.json");

    auto spriteRenderer = std::make_unique<SpriteRenderer>();

    auto renderer = std::make_unique<GameRenderer>(spriteRenderer.get(), spriteShader.get());

    auto ballParticleEmitter = std::make_unique<ParticleEmitter>(context->textureManager->getTexture("ball"), 1000);
    auto collisionHitParticleEmitter = std::make_unique<ParticleEmitter>(context->textureManager->getTexture("ball"), 300);

    Game game{{
        .contextPtr = context.get(),
        .spriteShaderPtr = spriteShader.get(),
        .textShaderPtr = textShader.get(),
        .particleShaderPtr = particleShader.get(),
        .gameRendererPtr = renderer.get(),
        .textRendererPtr = textRenderer.get(),
        .ballParticleEmitterPtr = ballParticleEmitter.get(),
        .collisionHitParticleEmitterPtr = collisionHitParticleEmitter.get(),
        .objectManagerPtr = objectManager.get(),
        .gameAttempts = 3,
    }};

    render::setUserPointer(&game);

    Canvas::init();
    Canvas canvas;
    resolveBuffer = render::getFramebuffer(1);
    msaaBuffer = render::getMultisamlpingFramebuffer(MSAA_SAMPLES);

    fastrand::getXorShiftState().offset = static_cast<uint32_t>(time(NULL));

    std::string shaders = context->pathManager->getResourcePath("shaders");
    Shader canvasShader(shaders + "/vs/canvas.glsl", shaders + "/fs/canvas.glsl", shaderObserver.get());
    canvasShader.setVec2("inverseScreenSize",
                         glm::vec2(1.0f / static_cast<float>(Window::getWidth()), 1.0f / static_cast<float>(Window::getHeight())));

    context->audioManager->loadBank("master");

    game.init();

    shaderObserver->startObserving();

    FPSCounter fpsCounter;

    float frameTime = 0.0f;
    float lastTime = 0.0f;
    float accumulation = 0.0f;
    while (!render::windowShouldClose()) {
        float currentTime = timer::time();
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
        context->audioManager->update();

        render::bindFramebuffer(0);
        canvasShader.use();
        canvas.render(canvasShader, resolveBuffer.attachments);
        game.renderText(alpha);

        render::pollWindowEvents();
        game.processInput(frameTime);

        // objectManager->cleanup();

        fpsCounter.update();

        render::setWindowTitle(std::format("{} ({} fps)", GAME_NAME, fpsCounter.getFPS()));

        render::swapBuffers();
        shaderObserver->update();
    }
    shaderObserver->stopObserving();
    render::terminate();
    logging::Log("Game is closed");
    return 0;
}
