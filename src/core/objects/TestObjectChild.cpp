#include "TestObjectChild.h"

void Vox::TestObjectChild::BuildProperties(std::vector<Property>& properties)
{
    TestObject::BuildProperties(properties);

    REGISTER_PROPERTY(unsigned int, test2)
}
