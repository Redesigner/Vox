#include "GBuffer.h"

#include <string>

#include <GL/glew.h>
#include <fmt/core.h>

#include "core/logging/Logging.h"
#include "rendering/SimpleFramebuffer.h"
#include "rendering/buffers/frame_buffers/Framebuffer.h"

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

        BindTexture(positionTexture, 0, GL_RGB32F, GL_RGB, GL_FLOAT);
        BindTexture(normalTexture, 1, GL_RGB32F, GL_RGB, GL_FLOAT);
        BindTexture(albedoTexture, 2, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
        BindTexture(metallicRoughnessTexture, 3, GL_RG, GL_RG, GL_FLOAT);
        BindTexture(depthTexture, 4, GL_R32F, GL_RED, GL_FLOAT);

        AttachDepthBuffer();

        GLenum buffersToDraw[5] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
        };
        glDrawBuffers(5, buffersToDraw);

        // VoxLog(Display, Rendering, "Position {}, Normal {}, AlbedoSpec {}, Depth {}", positionTexture, normalTexture, albedoTexture, depthTexture);
        CheckStatus();
    }

    GBuffer::~GBuffer()
    {
        VoxLog(Display, Rendering, "Destroying GBuffer...");
        // VoxLog(Display, Rendering, "Destroying GBuffer textures '({}, {}, {}, {}, {})'", positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture);

        const unsigned int textureIds[5] = {
            positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture
        };
        glDeleteTextures(5, textureIds);
    }

    void GBuffer::ActivateTextures(const unsigned int offset) const
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
