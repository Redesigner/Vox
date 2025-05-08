#include "PickShader.h"

#include "rendering/Camera.h"

namespace Vox
{
    PickShader::PickShader()
    {
        if (!Load(vertLocation, fragLocation))
        {
            return;
        }
        
        uniformLocations.modelMatrix = GetUniformLocation("matModel");
        uniformLocations.viewMatrix = GetUniformLocation("matView");
        uniformLocations.projectionMatrix = GetUniformLocation("matProjection");
        uniformLocations.objectId = GetUniformLocation("objectId");

        Enable();
    }

    void PickShader::SetCamera(Ref<Camera> camera) const
    {
        SetUniformMatrix(uniformLocations.viewMatrix, camera->GetViewMatrix());
        SetUniformMatrix(uniformLocations.projectionMatrix, camera->GetProjectionMatrix());
    }

    void PickShader::SetObjectId(int objectId) const
    {
        SetUniformUint(uniformLocations.objectId, objectId);
    }

    void PickShader::SetModelMatrix(const glm::mat4x4& matrix) const
    {
        SetUniformMatrix(uniformLocations.modelMatrix, matrix);
    }
}
