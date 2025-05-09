//
// Created by steph on 5/8/2025.
//

#pragma once
#include "rendering/buffers/frame_buffers/Framebuffer.h"

namespace Vox
{
    class UVec2Buffer : public Framebuffer
    {
    public:
        UVec2Buffer(int width, int height);
        ~UVec2Buffer();

        void ActivateTextures(const unsigned int offset) const;

    private:
        unsigned int uVec2Texture = 0;
    };
} // Vox
