#include "TestComponent.h"

namespace Vox
{
    TestComponent::TestComponent(Actor* parent)
        :Component(parent)
    {
    }

    void TestComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        Component::BuildProperties(propertiesInOut);
        REGISTER_PROPERTY(std::string, tag)
    }
}
