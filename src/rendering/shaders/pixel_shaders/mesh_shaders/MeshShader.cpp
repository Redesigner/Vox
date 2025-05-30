#include "MeshShader.h"

#include "../../../camera/Camera.h"

namespace Vox
{
    MeshShader::MeshShader(std::string vertLocation, std::string fragLocation)
        :PixelShader(vertLocation, fragLocation)
    {
        uniformLocations.modelMatrix = GetUniformLocation("matModel");
        uniformLocations.viewMatrix = GetUniformLocation("matView");
        uniformLocations.projectionMatrix = GetUniformLocation("matProjection");
    }

    void MeshShader::SetCamera(const std::shared_ptr<Camera>& camera) const
    {
        SetUniformMatrix(uniformLocations.viewMatrix, camera->GetViewMatrix());
        SetUniformMatrix(uniformLocations.projectionMatrix, camera->GetProjectionMatrix());
    }

    void MeshShader::SetModelMatrix(const glm::mat4x4& matrix) const
    {
        SetUniformMatrix(uniformLocations.modelMatrix, matrix);
    }
} // namespace Vox
