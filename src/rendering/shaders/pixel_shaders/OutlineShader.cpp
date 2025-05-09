//
// Created by steph on 5/8/2025.
//

#include "OutlineShader.h"

#include <glm/ext/matrix_transform.hpp>

namespace Vox
{
    OutlineShader::OutlineShader()
        :PixelShader(vertLocation, fragLocation)
    {
        uniformLocations.fragToPixelLocation = GetUniformLocation("fragToPixel");
        uniformLocations.pixelToFragLocation = GetUniformLocation("pixelToFrag");
        uniformLocations.stencilLocation = GetUniformLocation("stencil");

        Enable();
        SetUniformInt(uniformLocations.stencilLocation, 0);
    }

    void OutlineShader::SetTextureSize(glm::vec2 size) const
    {
        SetUniformVec2(uniformLocations.fragToPixelLocation, size);
        SetUniformVec2(uniformLocations.pixelToFragLocation, 1.0f / size);
    }
} // namespace Vox
