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

        void EnableFramebuffer();
        void BindDraw();
        void BindRead();

        void ActivateTextures(unsigned int offset = 0) const;

        void CopyToFramebuffer(const Texture& target);

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