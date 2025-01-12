#pragma once

namespace Vox
{
    class Texture;

    struct Shader;

    class GBuffer
    {
    public:
        GBuffer(int width, int height);
        ~GBuffer();

        void ActivateTextures(unsigned int offset = 0) const;

        void CopyToFramebuffer(unsigned int targetFramebuffer);

        unsigned int GetFramebufferId() const;

    private:

        unsigned int framebuffer;

        unsigned int positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture;

        unsigned int depthRenderbuffer;

        unsigned int width;
        unsigned int height;

        GBuffer(const GBuffer&) = delete;
        GBuffer& operator= (const GBuffer&) = delete;
    };
}