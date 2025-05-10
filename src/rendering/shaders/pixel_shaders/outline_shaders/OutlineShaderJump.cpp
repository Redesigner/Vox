//
// Created by steph on 5/8/2025.
//

#include "OutlineShaderJump.h"

namespace Vox
{
    OutlineShaderJump::OutlineShaderJump() : PixelShader(vertLocation, fragLocation)
    {
        uniformLocations.widthLocation = GetUniformLocation("width");
        uniformLocations.previousStageLocation = GetUniformLocation("previousStage");

        Enable();
        SetUniformInt(uniformLocations.previousStageLocation, 0);
    }

    void OutlineShaderJump::SetWidth(const int width) const
    {
        SetUniformInt(uniformLocations.widthLocation, width);
    }

} // namespace Vox
