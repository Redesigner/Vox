#include "GBufferShader.h"

#include "rendering/PBRMaterial.h"

namespace Vox
{
    GBufferShader::GBufferShader(const std::string& vertLocation, const std::string& fragLocation)
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
