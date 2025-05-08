#pragma once
#include "rendering/buffers/FrameBuffer.h"

namespace Vox
{
    class PickBuffer : public Framebuffer
    {
    public:
        PickBuffer(int widthIn, int heightIn);
        ~PickBuffer();

        PickBuffer(const PickBuffer&) = delete;
        PickBuffer(PickBuffer&&) = delete;
        PickBuffer& operator=(const PickBuffer&) = delete;
        PickBuffer& operator=(PickBuffer&&) = delete;

        void ActivateTextures(unsigned int offset) const;

        [[nodiscard]] unsigned int GetValue(int x, int y) const;

    private:
        unsigned int objectTexture = 0;
        unsigned int depthRenderbuffer = 0;
    };
}
