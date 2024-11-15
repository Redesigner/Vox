#pragma once

#include "raylib.h"

struct Shader;

class GBuffer
{
public:
    GBuffer(int width, int height);
    ~GBuffer();

    void EnableFramebuffer();

    void ActivateTextures() const;

    void CopyToFramebuffer(const RenderTexture2D& target);

private:

    unsigned int framebuffer;

    unsigned int positionTexture;
    unsigned int normalTexture;
    unsigned int albedoSpecTexture;

    unsigned int depthRenderbuffer;

    unsigned int width;
    unsigned int height;

    GBuffer (const GBuffer&) = delete;
    GBuffer& operator= (const GBuffer&) = delete;
};