#include "PickBuffer.h"

namespace Vox
{
    PickBuffer::PickBuffer(int widthIn, int heightIn)
        :Framebuffer(widthIn, heightIn)
    {
        glGenTextures(1, &objectTexture);
        glGenRenderbuffers(1, &depthRenderbuffer);

        BindTexture(objectTexture, 0, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
        glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        CheckStatus();
    }

    PickBuffer::~PickBuffer()
    {
        glDeleteTextures(1, &objectTexture);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
    }

    void PickBuffer::ActivateTextures(const unsigned int offset) const
    {
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, objectTexture);
    }

    unsigned int PickBuffer::GetValue(const unsigned int x, const unsigned int y) const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferId);
        unsigned int result = 0;
        glReadPixels(static_cast<int>(x), static_cast<int>(height - y), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &result);
        return result;
    }
}
