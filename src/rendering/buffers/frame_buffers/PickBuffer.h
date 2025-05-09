#pragma once
#include "rendering/buffers/frame_buffers/Framebuffer.h"

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

        [[nodiscard]] unsigned int GetValue(unsigned int x, unsigned int y) const;

    private:
        unsigned int objectTexture = 0;
    };
}
