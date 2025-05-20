//
// Created by steph on 5/20/2025.
//

#include "SkyShader.h"

#include "rendering/Camera.h"

namespace Vox
{
    SkyShader::SkyShader()
        :PixelShader(vertLocation, fragLocation)
    {
        Enable();
        SetUniformInt(GetUniformLocation("color"), 0);
        SetUniformInt(GetUniformLocation("depth"), 1);

        cameraWorldSpaceLocation = GetUniformLocation("cameraWorldSpace");
    }

    void SkyShader::SetCamera(const Ref<Camera>& camera) const
    {
        const glm::mat4x4 cameraRotation = camera->GetRotationMatrix();
        const glm::mat4x4 projection = camera->GetProjectionMatrix();
        const glm::mat4x4 matrix = glm::inverse(projection * cameraRotation);
        SetUniformMatrix(cameraWorldSpaceLocation, matrix);
    }
} // Vox