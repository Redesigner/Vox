#include "GBuffer.h"

#include <string>

#include <fmt/core.h>
#include <GL/glew.h>

#include "core/logging/Logging.h"
#include "rendering/SimpleFramebuffer.h"
#include "rendering/buffers/Framebuffer.h"

namespace Vox
{
    GBuffer::GBuffer(const int widthIn, const int heightIn)
        : Framebuffer(widthIn, heightIn)
    {
        unsigned int textureIds[5] = {};
        glGenTextures(5, textureIds);

        positionTexture = textureIds[0];
        normalTexture = textureIds[1];
        albedoTexture = textureIds[2];
        metallicRoughnessTexture = textureIds[3];
        depthTexture = textureIds[4];
        
        glGenRenderbuffers(1, &depthRenderbuffer);

        // Position buffer
        BindTexture(positionTexture, 0, GL_RGB32F, GL_RGB, GL_FLOAT);
        
        // Normal buffer
        BindTexture(normalTexture, 1, GL_RGB32F, GL_RGB, GL_FLOAT);

        // Albedo buffer
        BindTexture(albedoTexture, 2, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

        // Metallic/roughness buffer
        BindTexture(metallicRoughnessTexture, 3, GL_RG, GL_RG, GL_FLOAT);

        // Depth buffer (for reading)
        BindTexture(depthTexture, 4, GL_R32F, GL_RED, GL_FLOAT);

        // Alternate depth texture
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
        glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

        GLenum buffersToDraw[5] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
        };
        glDrawBuffers(5, buffersToDraw);

        // VoxLog(Display, Rendering, "Position {}, Normal {}, AlbedoSpec {}, Depth {}", positionTexture, normalTexture, albedoTexture, depthTexture);

        GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            VoxLog(Error, Rendering, "Failed to create GBuffer: {}",
                   SimpleFramebuffer::GetFramebufferStatusString(framebufferStatus));
        }
        else
        {
            VoxLog(Display, Rendering, "Successfully created GBuffer.");
        }
    }

    GBuffer::~GBuffer()
    {
        VoxLog(Display, Rendering, "Destroying GBuffer...");
        // VoxLog(Display, Rendering, "Destroying GBuffer textures '({}, {}, {}, {}, {})'", positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture);

        const unsigned int textureIds[5] = {
            positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture
        };
        glDeleteTextures(5, textureIds);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
    }

    void GBuffer::ActivateTextures(unsigned int offset) const
    {
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, positionTexture);

        glActiveTexture(GL_TEXTURE1 + offset);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        glActiveTexture(GL_TEXTURE2 + offset);
        glBindTexture(GL_TEXTURE_2D, albedoTexture);

        glActiveTexture(GL_TEXTURE3 + offset);
        glBindTexture(GL_TEXTURE_2D, metallicRoughnessTexture);

        glActiveTexture(GL_TEXTURE4 + offset);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
    }

    void GBuffer::CopyToFramebuffer(unsigned int targetFramebuffer)
    {
        glBlitNamedFramebuffer(framebufferId, targetFramebuffer, 0, 0, width, height, 0, 0, width, height,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_LINEAR);
    }
};
