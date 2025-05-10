//
// Created by steph on 5/10/2025.
//

#include "StencilBuffer.h"

namespace Vox
{
    StencilBuffer::StencilBuffer(int width, int height) : Framebuffer(width, height)
    {
        glGenTextures(1, &stencil);
        BindTexture(stencil, 0, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

        constexpr GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, buffers);

        CheckStatus();
    }

    StencilBuffer::~StencilBuffer()
    {
        glDeleteTextures(1, &stencil);
    }

    void StencilBuffer::ActivateTextures(const unsigned int offset) const
    {
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, stencil);
    }
} // Vox