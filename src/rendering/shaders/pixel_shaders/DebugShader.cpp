//
// Created by steph on 5/20/2025.
//

#include "DebugShader.h"

#include "rendering/Camera.h"

namespace Vox
{
    DebugShader::DebugShader(const std::string& vertLocation, const std::string& fragLocation)
        :PixelShader(vertLocation, fragLocation)
    {
        viewProjectionMatrixLocation = GetUniformLocation("viewProjection");
    }

    void DebugShader::SetCamera(const Ref<Camera>& camera) const
    {
        SetUniformMatrix(viewProjectionMatrixLocation, camera->GetViewProjectionMatrix());
    }
} // Vox