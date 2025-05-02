#pragma once

#include "core/objects/Object.h"
#include "core/objects/Property.h"

namespace Vox
{
    class TestObject : public Object
    {
    public:
        TestObject();
        
    protected:
        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
    private:
        float test = 0.0f;
        std::string testString = "this string is a member of TestObject";
        
        IMPLEMENT_PROPERTIES()
        IMPLEMENT_NAME(TestObject)
    };

    class TestObject2 : public Object
    {
    protected:
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

    private:
        bool test3 = true;
        float test4f = 222.0f;
        std::string testString2 = "this string is a member of TestObject2";
        
        IMPLEMENT_PROPERTIES()
        IMPLEMENT_NAME(TestObject2)
    };
}
