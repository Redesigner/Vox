//
// Created by steph on 5/10/2025.
//

#pragma once
#include "Framebuffer.h"

namespace Vox
{
    class StencilBuffer : public Framebuffer
    {
    public:
        StencilBuffer(int width, int height);
        ~StencilBuffer();

    private:
        unsigned int stencil = 0;
    };

}
