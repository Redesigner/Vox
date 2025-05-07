#include "GBufferShader.h"

#include "rendering/Camera.h"
#include "rendering/PBRMaterial.h"

namespace Vox
{
    GBufferShader::GBufferShader()
    {
        if (!Load(vertLocation, fragLocation))
        {
            return;
        }
        
        uniformLocations.modelMatrix = GetUniformLocation("matModel");
        uniformLocations.viewMatrix = GetUniformLocation("matView");
        uniformLocations.projectionMatrix = GetUniformLocation("matProjection");
        uniformLocations.roughness = GetUniformLocation("materialRoughness");
        uniformLocations.albedo = GetUniformLocation("materialAlbedo");

        Enable();
    }

    void GBufferShader::SetCamera(Ref<Camera> camera) const
    {
        SetUniformMatrix(uniformLocations.viewMatrix, camera->GetViewMatrix());
        SetUniformMatrix(uniformLocations.projectionMatrix, camera->GetProjectionMatrix());
    }

    void GBufferShader::SetMaterial(const PBRMaterial& material) const
    {
        SetUniformColor(uniformLocations.albedo, material.albedo);
        SetUniformFloat(uniformLocations.roughness, material.roughness);
    }

    void GBufferShader::SetModelMatrix(const glm::mat4x4& matrix) const
    {
        SetUniformMatrix(uniformLocations.modelMatrix, matrix);
    }
}
