#include "DeferredShader.h"

#include <fmt/format.h>

#include "rendering/Light.h"

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

        for (unsigned int i = 0; i < lightsPerPass; ++i)
        {
            auto& [enabled, type, position, target, color, strength] = lightLocations[i];
            enabled = GetUniformLocation(fmt::format("lights[{}].enabled", i));
            type = GetUniformLocation(fmt::format("lights[{}].type", i));
            position = GetUniformLocation(fmt::format("lights[{}].position", i));
            target = GetUniformLocation(fmt::format("lights[{}].target", i));
            color = GetUniformLocation(fmt::format("lights[{}].color", i));
            strength = GetUniformLocation(fmt::format("lights[{}].enabled", i).c_str());
        }
    }

    void DeferredShader::SetCameraPosition(const glm::vec3 position) const
    {
        SetUniformVec3(uniformLocations.viewPosition, position);
    }

    void DeferredShader::SetUniformLight(const Light& light, const unsigned int index) const
    {
        assert(index < lightsPerPass);

        // Send to shader light enabled state and type
        SetUniformInt(lightLocations[index].enabled, light.enabled);
        SetUniformInt(lightLocations[index].type, light.type);

        // Send to shader light position values
        SetUniformVec3(lightLocations[index].position, light.position);

        // Send to shader light target position values
        SetUniformVec3(lightLocations[index].target, light.target);

        // Send to shader light color values
        SetUniformColor(lightLocations[index].color, light.color);

        //shader->SetUniformFloat(uniformLocations.strength, strength);
    }
}
