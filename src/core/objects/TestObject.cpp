#include "TestObject.h"

namespace Vox
{
    void TestObject::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        REGISTER_PROPERTY(float, test)
    }
}
