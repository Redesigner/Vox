//
// Created by steph on 5/8/2025.
//
#pragma once
#include "rendering/shaders/pixel_shaders/PixelShader.h"

namespace Vox
{
    class OutlineShaderDistance : public PixelShader
    {
    public:
        OutlineShaderDistance();

        void SetWidth(int width) const;

        void SetOutlineColor(glm::vec3 color) const;

    private:
        struct UniformLocations
        {
            int widthLocation = -1;
            int previousStageLocation = -1;
            int colorLocation = -1;
            int outlineColorLocation = -1;
        };

        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/outlineFill.vert";
        static inline std::string fragLocation = "assets/shaders/outlineDistance.frag";
    };

} // Vox
