#pragma once

#include "glad/glad.h"
#include "logging.hpp"
#include "window.hpp"

struct BufferObject {
    unsigned int fbo;
    std::vector<unsigned int> attachments;
    size_t samples;

    BufferObject() : fbo(0), attachments({}) {}
    BufferObject(unsigned int buffer, std::vector<unsigned int> tex) : fbo(buffer), attachments(tex), samples(1) {}
    BufferObject(unsigned int buffer, std::vector<unsigned int> tex, size_t count) : fbo(buffer), attachments(tex), samples(count) {}
};

// RenderConfig
namespace render {

inline void setupDefaultAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

inline void setupMultisampling() {
    glEnable(GL_MULTISAMPLE);
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

inline BufferObject resizeFrambuffer(BufferObject oldBuffer) {
    glDeleteFramebuffers(1, &oldBuffer.fbo);
    for (auto &att : oldBuffer.attachments) {
        glDeleteTextures(1, &att);
    }
    unsigned int attachmentCount = static_cast<unsigned int>(oldBuffer.attachments.size());
    oldBuffer.attachments.clear();
    return getFramebuffer(attachmentCount);
}

inline BufferObject resizeMultisamplingFrambuffer(BufferObject oldBuffer) {
    glDeleteFramebuffers(1, &oldBuffer.fbo);
    for (auto &att : oldBuffer.attachments) {
        glDeleteTextures(1, &att);
    }
    return getMultisamlpingFramebuffer(oldBuffer.samples);
}

} // namespace render
