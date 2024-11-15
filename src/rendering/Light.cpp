#include "Light.h"
#include "Light.h"
#include "Light.h"
#include "Light.h"

#include "raylib.h"

Light::Light()
    :enabled(false), type(0), position(), target(), color()
{
}

Light::Light(int enabled, int type, Vector3 position, Vector3 target, Vector4 color, float distance)
    :enabled(enabled), type(type), position(position), target(target), color(color), strength(distance)
{
}

void Light::UpdateLightValues(Shader& shader, LightUniformLocations& uniformLocations)
{
    // Send to shader light enabled state and type
    SetShaderValue(shader, uniformLocations.enabled, &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, uniformLocations.type, &type, SHADER_UNIFORM_INT);

    // Send to shader light position values
    float positionPacked[3] = { position.x, position.y, position.z };
    SetShaderValue(shader, uniformLocations.position, positionPacked, SHADER_UNIFORM_VEC3);

    // Send to shader light target position values
    float targetPacked[3] = {target.x, target.y, target.z };
    SetShaderValue(shader, uniformLocations.target, targetPacked, SHADER_UNIFORM_VEC3);

    // Send to shader light color values
    float colorPacked[4] = { (float)color.x / (float)255, (float)color.y / (float)255,
                        (float)color.z / (float)255, (float)color.w / (float)255 };
    SetShaderValue(shader, uniformLocations.color, colorPacked, SHADER_UNIFORM_VEC4);

    SetShaderValue(shader, uniformLocations.strength, &strength, SHADER_UNIFORM_FLOAT);
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

LightUniformLocations::LightUniformLocations(Shader& shader)
{
    int lightsCount = 1;
    enabled = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightsCount));
    type = GetShaderLocation(shader, TextFormat("lights[%i].type", lightsCount));
    position = GetShaderLocation(shader, TextFormat("lights[%i].position", lightsCount));
    target = GetShaderLocation(shader, TextFormat("lights[%i].target", lightsCount));
    color = GetShaderLocation(shader, TextFormat("lights[%i].color", lightsCount));
    strength = GetShaderLocation(shader, TextFormat("lights[%i].strength", lightsCount));
}
