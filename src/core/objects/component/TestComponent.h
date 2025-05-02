#pragma once
#include "Component.h"

namespace Vox
{
    class TestComponent : public Component
    {
    public:
        TestComponent();
        
    private:
        IMPLEMENT_PROPERTIES()
        IMPLEMENT_NAME(TestComponent)
    };
}
