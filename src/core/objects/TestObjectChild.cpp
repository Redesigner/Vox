#include "TestObjectChild.h"

void Vox::TestObjectChild::BuildProperties(std::vector<Property>& propertiesInOut)
{
    TestObject::BuildProperties(propertiesInOut);
    TestObject2::BuildProperties(propertiesInOut);
    
    REGISTER_PROPERTY(float, test2)
    REGISTER_PROPERTY(std::string, testString3)
}
