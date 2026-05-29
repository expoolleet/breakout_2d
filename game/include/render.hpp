#pragma once
#include <GLFW/glfw3.h>

#include "glad/glad.h"
#include "logging.hpp"
#include "render_type.hpp"
#include "window.hpp"

struct BufferObject {
    unsigned int fbo;
    std::vector<unsigned int> attachments;
    size_t samples;

    BufferObject() : fbo(0), attachments({}) {}
    BufferObject(unsigned int buffer, std::vector<unsigned int> texs) : fbo(buffer), attachments(texs), samples(1) {}
    BufferObject(unsigned int buffer, std::vector<unsigned int> texs, size_t count) : fbo(buffer), attachments(texs), samples(count) {}
};

// Render
namespace render {

inline RenderType type = RenderType::Unknown;

inline GLFWwindow *window = nullptr;

inline void initWindow() {
    if (type == RenderType::Unknown) {
        logging::Error("Renderer type was not initialized!");
        return;
    }

    if (type == RenderType::OpenGL) {
        glfwInit();
        glfwWindowHint(GLFW_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_VERSION_MINOR, 5);
        window = Window::createWindow("Breakout2D", 1280, 720);
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

inline void initAPI() {
    if (type == RenderType::Unknown) {
        logging::Error("Renderer type was not initialized!");
        return;
    }

    if (type == RenderType::OpenGL && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logging::Error("Could not load glad!");
        return;
    }
}

inline void setUserPointer(void *ptr) {
    glfwSetWindowUserPointer(window, ptr);
}

inline void *getUserPointer() {
    return glfwGetWindowUserPointer(window);
}

inline void setFrameBufferSizeCallback(auto &&cb) {
    glfwSetFramebufferSizeCallback(window, cb);
}

inline void setKeyCallback(auto &&cb) {
    glfwSetKeyCallback(window, cb);
}

inline bool windowShouldClose() {
    return glfwWindowShouldClose(window);
}

inline void swapBuffers() {
    if (type == RenderType::OpenGL) {
        glfwSwapBuffers(window);
    }
}

inline void setViewport() {
    glViewport(0, 0, Window::getWidth(), Window::getHeight());
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

inline void enableVSync(bool enabled) {
    glfwSwapInterval(static_cast<int>(enabled));
}

inline void setupDefaultAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

inline void setupMultisampling() {
    glEnable(GL_MULTISAMPLE);
}

inline void pollWindowEvents() {
    glfwPollEvents();
}

inline void bindFramebuffer(unsigned int frambuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, frambuffer);
    glClear(GL_COLOR_BUFFER_BIT);
}

inline void blitFramebuffer(unsigned int readBuffer, unsigned int drawBuffer) {
    glBlitNamedFramebuffer(readBuffer, drawBuffer, 0, 0, Window::getWidth(), Window::getHeight(), 0, 0, Window::getWidth(),
                           Window::getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

inline BufferObject getMultisamlpingFramebuffer(size_t samples) {
    unsigned int fbo = 0;
    glCreateFramebuffers(1, &fbo);
    unsigned int colorAttachment = 0;
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &colorAttachment);
    glTextureStorage2DMultisample(colorAttachment, samples, GL_RGB16F, Window::getWidth(), Window::getHeight(), GL_TRUE);
    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, colorAttachment, 0);

    if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logging::Error("Could not create multisampled framebuffer");
    }

    return BufferObject(fbo, {colorAttachment});
}

inline BufferObject getFramebuffer(unsigned int colorAttachmentCount) {
    unsigned int fbo = 0;
    glCreateFramebuffers(1, &fbo);
    std::vector<unsigned int> colorAttachments;
    std::vector<unsigned int> drawBuffers;
    for (unsigned int i = 0; i < colorAttachmentCount; ++i) {
        unsigned int tex = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &tex);
        glTextureStorage2D(tex, 1, GL_RGB16F, Window::getWidth(), Window::getHeight());
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + i, tex, 0);
        colorAttachments.push_back(tex);
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glNamedFramebufferDrawBuffers(fbo, colorAttachmentCount, drawBuffers.data());

    return BufferObject(fbo, colorAttachments);
}

inline void resizeFrambuffer(BufferObject &buffer) {
    glDeleteFramebuffers(1, &buffer.fbo);
    for (auto &att : buffer.attachments) {
        glDeleteTextures(1, &att);
    }
    unsigned int attachmentCount = static_cast<unsigned int>(buffer.attachments.size());
    buffer.attachments.clear();
    buffer = getFramebuffer(attachmentCount);
}

inline void resizeMultisamplingFrambuffer(BufferObject &buffer) {
    glDeleteFramebuffers(1, &buffer.fbo);
    for (auto &att : buffer.attachments) {
        glDeleteTextures(1, &att);
    }
    buffer = getMultisamlpingFramebuffer(buffer.samples);
}

inline void setWindowTitle(const std::string &title) {
    glfwSetWindowTitle(window, title.data());
}

inline void terminate() {
    glfwTerminate();
}

}  // namespace render
