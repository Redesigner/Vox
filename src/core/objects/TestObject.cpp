#include "TestObject.h"

namespace Vox
{
    void TestObject::BuildProperties(std::vector<Property>& properties)
    {
        REGISTER_PROPERTY(float, test)
    }
}