//
// Created by steph on 5/8/2025.
//
#pragma once
#include "../mesh_shaders/MeshShader.h"

namespace Vox {

    class OutlineShader : public PixelShader
    {
    public:
        OutlineShader();

        void SetTextureSize(glm::vec2 size) const;

    private:
        struct UniformLocations
        {
            int fragToPixelLocation = -1;
            int pixelToFragLocation = -1;
            int stencilLocation = -1;
        };

        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/outline/outlineFill.vert";
        static inline std::string fragLocation = "assets/shaders/outline/outlineFill.frag";
    };

} // Vox
