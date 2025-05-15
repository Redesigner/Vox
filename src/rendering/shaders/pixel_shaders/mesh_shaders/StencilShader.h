//
// Created by steph on 5/10/2025.
//

#pragma once
#include "MeshShader.h"

namespace Vox
{
    class StencilShader : public MeshShader
    {
    public:
        StencilShader();
        StencilShader(const std::string& vertLocation, const std::string& fragLocation);

    private:
        static inline std::string vertLocation = "assets/shaders/stencil.vert";
        static inline std::string fragLocation = "assets/shaders/stencil.frag";
    };

} // Vox
