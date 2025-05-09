#include "RenderTexture.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"
#include "rendering/SimpleFramebuffer.h"

namespace Vox
{
    RenderTexture::RenderTexture(int width, int height)
        : width(width), height(height)
    {
        VoxLog(Display, Rendering, "Allocating new framebuffer for RenderTexture: size ({}, {}).", width, height);

        if (width == 0 || height == 0)
        {
            framebuffer = texture = 0;
            VoxLog(Warning, Rendering, "Failed to create renderTexture, width or height cannot be 0.");
            return;
        }

        glGenFramebuffers(1, &framebuffer); // Create the framebuffer object
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        if (const GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); framebufferStatus !=
            GL_FRAMEBUFFER_COMPLETE)
        {
            VoxLog(Error, Rendering, "Failed to create renderTexture: {}",
                SimpleFramebuffer::GetFramebufferStatusString(framebufferStatus));
        }
    }

    RenderTexture::~RenderTexture()
    {
        glDeleteTextures(1, &texture);
        glDeleteFramebuffers(1, &framebuffer);
    }

    unsigned int RenderTexture::GetFramebufferId() const
    {
        return framebuffer;
    }

    unsigned int RenderTexture::GetTextureId() const
    {
        return texture;
    }

    int RenderTexture::GetWidth() const
    {
        return width;
    }

    int RenderTexture::GetHeight() const
    {
        return height;
    }

    double RenderTexture::GetAspectRatio() const
    {
        return static_cast<double>(height) / static_cast<double>(width);
    }
}
