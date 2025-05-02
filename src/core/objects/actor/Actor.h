#pragma once
#include "core/objects/Object.h"

namespace  Vox
{
    class Actor : public Object
    {
    public:

        IMPLEMENT_PROPERTIES()
        IMPLEMENT_NAME(Actor)
    };
}
