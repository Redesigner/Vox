//
// Created by steph on 5/8/2025.
//

#include "OutlineShaderDistance.h"

namespace Vox
{
    OutlineShaderDistance::OutlineShaderDistance() : PixelShader(vertLocation, fragLocation)
    {
        uniformLocations.widthLocation = GetUniformLocation("width");
        uniformLocations.previousStageLocation = GetUniformLocation("previousStage");
        uniformLocations.colorLocation = GetUniformLocation("color");
        uniformLocations.outlineColorLocation = GetUniformLocation("outlineColor");

        Enable();
        SetUniformInt(uniformLocations.previousStageLocation, 0);
        SetUniformInt(uniformLocations.colorLocation, 1);
    }

    void OutlineShaderDistance::SetWidth(const int width) const
    {
        SetUniformInt(uniformLocations.widthLocation, width);
    }

    void OutlineShaderDistance::SetOutlineColor(glm::vec3 color) const
    {
        SetUniformVec3(uniformLocations.outlineColorLocation, color);
    }

} // namespace Vox
