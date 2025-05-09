//
// Created by steph on 5/8/2025.
//

#include "UVec2Buffer.h"

namespace Vox
{
    UVec2Buffer::UVec2Buffer(const int width, const int height)
        :Framebuffer(width, height)
    {
        glGenTextures(1, &uVec2Texture);
        BindTexture(uVec2Texture, 0, GL_RG16I, GL_RG_INTEGER, GL_SHORT);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        CheckStatus();
    }

    UVec2Buffer::~UVec2Buffer()
    {
        glDeleteTextures(0, &uVec2Texture);
    }

    void UVec2Buffer::ActivateTextures(const unsigned int offset) const
    {
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, uVec2Texture);
    }
} // Vox