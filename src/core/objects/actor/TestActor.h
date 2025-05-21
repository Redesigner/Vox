#pragma once
#include "Actor.h"

namespace Vox
{
    class TestActor : public Actor
    {
    public:
        TestActor(World* world);

        IMPLEMENT_OBJECT(TestActor)
        
    };
}
