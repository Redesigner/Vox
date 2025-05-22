#pragma once
#include "Component.h"

namespace Vox
{
    class TestComponent : public Component
    {
    public:
        TestComponent(const ObjectInitializer& objectInitializer);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
    private:
        IMPLEMENT_OBJECT(TestComponent)

        std::string tag;
    };
}
