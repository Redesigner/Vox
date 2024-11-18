#pragma once

#include "raylib.h"

class DeferredShader;

struct LightUniformLocations
{
    LightUniformLocations();
    LightUniformLocations(DeferredShader* shader);

    int enabled;
    int type;
    int position;
    int target;
    int color;
    int strength;
};

struct Light
{
    Light();
    Light(int enabled, int type, Vector3 position, Vector3 target, Vector4 color, float distance);

    int enabled;
    int type;
    Vector3 position;
    Vector3 target;
    Vector4 color;
    float strength;

    void UpdateLightValues(DeferredShader* shader, LightUniformLocations& uniformLocations);
};