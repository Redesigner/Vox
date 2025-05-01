﻿#pragma once
#include "core/objects/Object.h"

namespace Vox
{
    class DetailPanel
    {
    public:
        static void Draw(Object* object);

        static void DrawProperty(Object* object, Property& property);
    };
}
