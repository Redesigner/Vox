#include "DeferredShader.h"

namespace Vox
{
    DeferredShader::DeferredShader()
        :PixelShader(vertLocation, fragLocation)
    {
        uniformLocations.position = GetUniformLocation("gPosition");
        uniformLocations.normal = GetUniformLocation("gNormal");
        uniformLocations.albedo = GetUniformLocation("gAlbedo");
        uniformLocations.metallicRoughness = GetUniformLocation("gMetallicRoughness");
        uniformLocations.depth = GetUniformLocation("depth");
        uniformLocations.viewPosition = GetUniformLocation("viewPosition");

        Enable();
        SetUniformInt(uniformLocations.position, 0);
        SetUniformInt(uniformLocations.normal, 1);
        SetUniformInt(uniformLocations.albedo, 2);
        SetUniformInt(uniformLocations.metallicRoughness, 3);
        SetUniformInt(uniformLocations.depth, 4);
    }

    void DeferredShader::SetCameraPosition(glm::vec3 position)
    {
        SetUniformVec3(uniformLocations.viewPosition, position);
    }
}
