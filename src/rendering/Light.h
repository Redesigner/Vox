#pragma once

#include "raylib.h"

struct Light
{
    int enabled;
    int type;
    Vector3 position;
    Vector3 target;
    Vector4 color;
};