#include "Light.h"

#include <fmt/format.h>

#include "rendering/shaders/pixel_shaders/DeferredShader.h"

namespace Vox
{
    Light::Light()
        :enabled(false), type(0), position(), target(), color(), strength(0.0f)
    {
    }

    Light::Light(int enabled, int type, glm::vec3 position, glm::vec3 target, glm::vec4 color, float distance)
        :enabled(enabled), type(type), position(position), target(target), color(color), strength(distance)
    {
    }

    void Light::UpdateLightValues(DeferredShader* shader, LightUniformLocations& uniformLocations)
    {
        // Send to shader light enabled state and type
        shader->SetUniformInt(uniformLocations.enabled, enabled);
        shader->SetUniformInt(uniformLocations.type, type);

        // Send to shader light position values
        shader->SetUniformVec3(uniformLocations.position, position);

        // Send to shader light target position values
        shader->SetUniformVec3(uniformLocations.target, target);

        // Send to shader light color values
        shader->SetUniformColor(uniformLocations.color, color);

        //shader->SetUniformFloat(uniformLocations.strength, strength);
    }

    LightUniformLocations::LightUniformLocations()
    {
        enabled = -1;
        type = -1;
        position = -1;
        target = -1;
        color = -1;
        strength = -1;
    }

    LightUniformLocations::LightUniformLocations(DeferredShader* shader)
    {
        int lightsCount = 1;
        enabled = shader->GetUniformLocation(fmt::format("lights[{}].enabled", lightsCount));
        type = shader->GetUniformLocation(fmt::format("lights[{}].type", lightsCount));
        position = shader->GetUniformLocation(fmt::format("lights[{}].position", lightsCount));
        target = shader->GetUniformLocation(fmt::format("lights[{}].target", lightsCount));
        color = shader->GetUniformLocation(fmt::format("lights[{}].color", lightsCount));
        strength = shader->GetUniformLocation(fmt::format("lights[{}].enabled", lightsCount).c_str());
    }
}