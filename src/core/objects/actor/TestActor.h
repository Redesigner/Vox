#pragma once
#include "Actor.h"

namespace Vox
{
    class TestActor : public Actor
    {
    public:
        TestActor();

        IMPLEMENT_NAME(TestActor)
        IMPLEMENT_PROPERTIES()
        
    };
}
