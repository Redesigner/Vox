#include "MaterialShader.h"

#include "rendering/PBRMaterial.h"

namespace Vox
{
    MaterialShader::MaterialShader(const std::string& vertLocation, const std::string& fragLocation)
        :MeshShader(vertLocation, fragLocation)
    {
        uniformLocations.roughness = GetUniformLocation("materialRoughness");
        uniformLocations.albedo = GetUniformLocation("materialAlbedo");
    }

    void MaterialShader::SetMaterial(const PBRMaterial& material) const
    {
        SetUniformColor(uniformLocations.albedo, material.albedo);
        SetUniformFloat(uniformLocations.roughness, material.roughness);
    }
}
