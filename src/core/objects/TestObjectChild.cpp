#include "TestObjectChild.h"

void Vox::TestObjectChild::BuildProperties(std::vector<Property>& propertiesInOut)
{
    TestObject::BuildProperties(properties);

    REGISTER_PROPERTY(unsigned int, test2)
}
