#pragma once
#include "TestObject.h"

namespace Vox
{
    class TestObjectChild : public TestObject
    {
    protected:
        void BuildProperties(std::vector<Property>& properties) override;
        
    private:
        unsigned int test2 = 0;

        IMPLEMENT_PROPERTIES()
    };
}
