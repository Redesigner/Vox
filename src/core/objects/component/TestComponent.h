#pragma once
#include "Component.h"

namespace Vox
{
    class TestComponent : public Component
    {
    public:
        TestComponent();

        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
    private:
        IMPLEMENT_OBJECT(TestComponent)

        std::string tag;
    };
}
