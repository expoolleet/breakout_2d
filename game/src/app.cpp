#include "app.hpp"

#include "audio_manager.hpp"
#include "canvas.hpp"
#include "fast_random.hpp"
#include "fps_counter.hpp"
#include "game.hpp"
#include "game_renderer.hpp"
#include "render.hpp"
#include "sprite_renderer.hpp"
#include "texture_literals.hpp"
#include "timer.hpp"

using namespace texture_literals;

constexpr float FIXED_FRAMETIME = 1.0f / 100.0f;  // 100 Hz fixed update loop
constexpr float MAX_FRAMETIME = 0.25f;
constexpr int MSAA_SAMPLES = 4;

struct GLFWUserCallbackArgs {
    Game *gamePtr;
    BufferObject *resolveBufferPtr;
    BufferObject *msaaBufferPtr;
};

static void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto args = static_cast<GLFWUserCallbackArgs *>(render::getUserPointer());
    if (action == GLFW_PRESS) {
        args->gamePtr->keys.press(key);
    } else if (action == GLFW_RELEASE) {
        args->gamePtr->keys.unpress(key);
    }
#ifdef _DEBUG
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        args->gamePtr->spawnBall(glm::vec2(0.0f));
    }
#endif
}

static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto args = static_cast<GLFWUserCallbackArgs *>(render::getUserPointer());
    if (width < 1 || height < 1) return;
    Window::setWidth(width);
    Window::setHeight(height);
    render::setViewport();
    args->gamePtr->setProjectionMatrix();
    render::resizeFrambuffer(*(args->resolveBufferPtr));
    render::resizeMultisamplingFrambuffer(*(args->msaaBufferPtr));
}

App::App() : objectManager(&context) {
    std::string texturesPath = context.getPathManager().getResourcePath("textures");
    TextureManager &textureManager = context.getTextureManager();
    textureManager.loadTexture(texturesPath + "/background_2.png", false, BACKGROUND_TEXTURE);
    textureManager.loadTexture(texturesPath + "/brick_standard.png", true, BRICK_TEXTURE);
    textureManager.loadTexture(texturesPath + "/brick_undestroyable.png", true, BADROCK_TEXTURE);
    textureManager.loadTexture(texturesPath + "/ball.png", true, "ball");
    textureManager.loadTexture(texturesPath + "/player_skin_2.png", true, PLAYER_TEXTURE);

    Canvas::init();

    fastrand::getXorShiftState().offset = static_cast<uint32_t>(time(NULL));
}

void App::run() {
    render::setFrameBufferSizeCallback(framebufferResizeCallback);
    render::setKeyCallback(keyCallback);
    render::setupDefaultAlphaBlending();
    render::setupMultisampling();
    render::enableVSync(true);

    std::string shadersPath = context.getPathManager().getResourcePath("shaders");

    Shader spriteShader{shadersPath + "/vs/sprite.glsl", shadersPath + "/fs/sprite.glsl", &shaderObserver};
    spriteShader.setMat4("view", glm::mat4(1.0f));
    spriteShader.setInt("sprite", 0);

    Shader particleShader{shadersPath + "/vs/particle.glsl", shadersPath + "/fs/particle.glsl", &shaderObserver};
    particleShader.setMat4("view", glm::mat4(1.0f));
    particleShader.setInt("sprite", 0);

    Shader textShader{shadersPath + "/vs/text.glsl", shadersPath + "/fs/textMSDF.glsl", &shaderObserver};

    std::string fontsPath = context.getPathManager().getResourcePath("fonts");
    TextRenderer textRenderer{fontsPath};
    textRenderer.initRenderer();
    textRenderer.initFontMSDF(fontsPath + "/msdf/roboto/atlas.png", fontsPath + "/msdf/roboto/atlas.json");

    SpriteRenderer spriteRenderer;
    GameRenderer renderer{&spriteRenderer, &spriteShader};

    ParticleEmitter ballParticleEmitter{context.getTextureManager().getTexture("ball"), 1000};
    ParticleEmitter collisionHitParticleEmitter{context.getTextureManager().getTexture("ball"), 300};

    Game game{{
        .contextPtr = &context,
        .spriteShaderPtr = &spriteShader,
        .textShaderPtr = &textShader,
        .particleShaderPtr = &particleShader,
        .gameRendererPtr = &renderer,
        .textRendererPtr = &textRenderer,
        .ballParticleEmitterPtr = &ballParticleEmitter,
        .collisionHitParticleEmitterPtr = &collisionHitParticleEmitter,
        .objectManagerPtr = &objectManager,
        .gameAttempts = 3,
    }};

    BufferObject resolveBuffer = render::getFramebuffer(1);
    BufferObject msaaBuffer = render::getMultisamlpingFramebuffer(MSAA_SAMPLES);

    GLFWUserCallbackArgs data{
        .gamePtr = &game,
        .resolveBufferPtr = &resolveBuffer,
        .msaaBufferPtr = &msaaBuffer,
    };

    render::setUserPointer(&data);

    game.init();

    context.getAudioManager().loadBank("master");

    shaderObserver.startObserving();

    FPSCounter fpsCounter;

    std::string shaders = context.getPathManager().getResourcePath("shaders");
    Shader canvasShader(shaders + "/vs/canvas.glsl", shaders + "/fs/canvas.glsl", &shaderObserver);
    canvasShader.setVec2("inverseScreenSize",
                         glm::vec2(1.0f / static_cast<float>(Window::getWidth()), 1.0f / static_cast<float>(Window::getHeight())));

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
        context.getAudioManager().update();

        render::bindFramebuffer(0);
        canvasShader.use();
        canvas.render(canvasShader, resolveBuffer.attachments);
        game.renderText(alpha);

        render::pollWindowEvents();
        game.processInput(frameTime);

        // objectManager.cleanup();

        fpsCounter.update();

        render::setWindowTitle(std::format("{} ({} fps)", GAME_NAME, fpsCounter.getFPS()));

        render::swapBuffers();
        shaderObserver.update();
    }
    shaderObserver.stopObserving();
}
