#include "GBufferShader.h"

#include "rendering/Camera.h"
#include "rendering/PBRMaterial.h"

namespace Vox
{
    GBufferShader::GBufferShader()
        :MeshShader(vertLocation, fragLocation)
    {
        uniformLocations.roughness = GetUniformLocation("materialRoughness");
        uniformLocations.albedo = GetUniformLocation("materialAlbedo");
    }

    void GBufferShader::SetMaterial(const PBRMaterial& material) const
    {
        SetUniformColor(uniformLocations.albedo, material.albedo);
        SetUniformFloat(uniformLocations.roughness, material.roughness);
    }
}
