//
// Created by steph on 5/8/2025.
//
#pragma once
#include "rendering/shaders/pixel_shaders/PixelShader.h"

namespace Vox
{
    class OutlineShaderJump : public PixelShader
    {
    public:
        OutlineShaderJump();

        void SetWidth(int width) const;

    private:
        struct UniformLocations
        {
            int widthLocation = -1;
            int previousStageLocation = -1;
        };

        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/outlineFill.vert";
        static inline std::string fragLocation = "assets/shaders/outlineJump.frag";
    };

} // Vox
