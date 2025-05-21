#include "TestObjectChild.h"

namespace Vox
{
    TestObjectChild::TestObjectChild(Object* parent)
        :TestObject(parent)
    {
        displayName = fmt::format("{}_Default", TestObjectChild::classDisplayName);
    }

    void TestObjectChild::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        TestObject::BuildProperties(propertiesInOut);

        REGISTER_PROPERTY(float, test2)
        REGISTER_PROPERTY(std::string, testString3)
        REGISTER_PROPERTY(glm::vec3, position)
    }
}
