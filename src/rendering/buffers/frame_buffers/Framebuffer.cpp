#include "Framebuffer.h"

#include <GL/glew.h>
#include <cassert>

#include "core/logging/Logging.h"
#include "rendering/SimpleFramebuffer.h"

namespace Vox
{
    Framebuffer::Framebuffer(int width, int height)
        :width(width), height(height)
    {
        CreateFrameBuffer();
    }

    Framebuffer::~Framebuffer()
    {
        if (depthRenderbuffer)
        {
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            glDeleteFramebuffers(1, &framebufferId);
        }
    }

    unsigned int Framebuffer::GetFramebufferId() const
    {
        return framebufferId;
    }

    glm::ivec2 Framebuffer::GetSize() const
    {
        return {width, height};
    }

    void Framebuffer::CreateFrameBuffer()
    {
        if (width == 0 || height == 0)
        {
            VoxLog(Warning, Rendering, "Failed to create FrameBuffer, width or height cannot be 0.");
            return;
        }
        
        glGenFramebuffers(1, &framebufferId); // Create the framebuffer object
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId); // Bind the new FBO
    }

    void Framebuffer::BindTexture(unsigned int textureId, unsigned int attachment, GLint internalFormat, GLenum format,
                                  GLenum type) const
    {
        assert(attachment < GL_MAX_COLOR_ATTACHMENTS);

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
        glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0 + attachment, textureId, 0);
    }

    void Framebuffer::AttachDepthBuffer()
    {
        glGenRenderbuffers(1, &depthRenderbuffer);

        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
        glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    }

    bool Framebuffer::CheckStatus()
    {
        if (GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            VoxLog(Error, Rendering, "Failed to create FrameBuffer: {}",
                   SimpleFramebuffer::GetFramebufferStatusString(framebufferStatus));
            return false;
        }
        VoxLog(Display, Rendering, "Successfully created FrameBuffer.");
        return true;
    }
}
