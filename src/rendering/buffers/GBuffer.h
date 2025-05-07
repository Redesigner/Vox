#pragma once
#include "Framebuffer.h"

namespace Vox
{
    class Texture;

    struct Shader;

    class GBuffer : public Framebuffer
    {
    public:
        GBuffer(int widthIn, int heightIn);
        ~GBuffer();

        GBuffer(const GBuffer&) = delete;

        GBuffer& operator=(const GBuffer&) = delete;
        
        void ActivateTextures(unsigned int offset = 0) const;

        void CopyToFramebuffer(unsigned int targetFramebuffer);

    private:
        unsigned int positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture;

        unsigned int depthRenderbuffer;
    };
}
