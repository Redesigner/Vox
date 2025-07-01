#pragma once
#include "Actor.h"

namespace Vox
{
    class TestActor : public Actor
    {
    public:
        TestActor(const ObjectInitializer&);

        IMPLEMENT_OBJECT(TestActor, Actor)
        
    };
}
