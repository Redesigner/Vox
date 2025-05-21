#include "TestObject.h"

namespace Vox
{
    TestObject::TestObject(Object* parent)
    {
        displayName = fmt::format("{}_Default", classDisplayName);
    }

    void TestObject::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        REGISTER_PROPERTY(float, test)
        REGISTER_PROPERTY(std::string, testString)
    }

    void TestObject2::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        REGISTER_PROPERTY(bool, test3)
        REGISTER_PROPERTY(float, test4f)
        REGISTER_PROPERTY(std::string, testString2)
    }
}
