//
// Created by steph on 5/10/2025.
//

#include "StencilBuffer.h"

namespace Vox
{
    StencilBuffer::StencilBuffer(int width, int height) : Framebuffer(width, height)
    {
        glGenTextures(1, &stencil);
        BindTexture(stencil, 0, GL_STENCIL_INDEX, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

        constexpr GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, buffers);

        CheckStatus();
    }

    StencilBuffer::~StencilBuffer()
    {
        glDeleteTextures(1, &stencil);
    }
} // Vox