//
// Created by steph on 5/10/2025.
//

#include "StencilShader.h"

namespace Vox
{
    StencilShader::StencilShader()
        :MeshShader(vertLocation, fragLocation)
    {
    }

    StencilShader::StencilShader(const std::string& vertLocation, const std::string& fragLocation)
        :MeshShader(vertLocation, fragLocation)
    {
    }
} // Vox