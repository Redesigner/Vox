#include "TestComponent.h"

namespace Vox
{
    TestComponent::TestComponent()
    {
        DEFAULT_DISPLAY_NAME()
    }

    void TestComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        Component::BuildProperties(propertiesInOut);
        REGISTER_PROPERTY(std::string, tag)
    }
}
