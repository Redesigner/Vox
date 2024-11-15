#pragma once

#include "raylib.h"

class Shader;

struct LightUniformLocations
{
    LightUniformLocations();
    LightUniformLocations(Shader& shader);

    int enabled;
    int type;
    int position;
    int target;
    int color;
    int attenuation;
};

struct Light
{
    Light();
    Light(int enabled, int type, Vector3 position, Vector3 target, Vector4 color);

    int enabled;
    int type;
    Vector3 position;
    Vector3 target;
    Vector4 color;

    void UpdateLightValues(Shader& shader, LightUniformLocations& uniformLocations);
};