#include "Light.h"

#include "raylib.h"

#include "rendering/shaders/DeferredShader.h"

Light::Light()
    :enabled(false), type(0), position(), target(), color(), strength(0.0f)
{
}

Light::Light(int enabled, int type, Vector3 position, Vector3 target, Vector4 color, float distance)
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

    shader->SetUniformFloat(uniformLocations.strength, strength);
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
    enabled = shader->GetUniformLocation(TextFormat("lights[%i].enabled", lightsCount));
    type = shader->GetUniformLocation(TextFormat("lights[%i].type", lightsCount));
    position = shader->GetUniformLocation(TextFormat("lights[%i].position", lightsCount));
    target = shader->GetUniformLocation(TextFormat("lights[%i].target", lightsCount));
    color = shader->GetUniformLocation(TextFormat("lights[%i].color", lightsCount));
    strength = shader->GetUniformLocation(TextFormat("lights[%i].strength", lightsCount));
}
