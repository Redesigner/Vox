#pragma once
#include "TestObject.h"

namespace Vox
{
    class TestObjectChild : public TestObject, public TestObject2
    {
    protected:
        void BuildProperties(std::vector<Property>& properties) override;
        
    private:
        float test2 = 5.0f;
        std::string testString3 = "this string is a member of TestObjectChild";

        IMPLEMENT_PROPERTIES()
    };
}
