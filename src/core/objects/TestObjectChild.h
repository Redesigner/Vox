#pragma once
#include "TestObject.h"

namespace Vox
{
    class ITestInterface
    {
    public:
        virtual void doSomething() = 0;
    };
    
    class TestObjectChild : public ITestInterface, public TestObject
    {
    public:
        void doSomething() override {};
        
    protected:
        void BuildProperties(std::vector<Property>& properties) override;
        
    private:
        float test2 = 5.0f;
        std::string testString3 = "this string is a member of TestObjectChild";

        IMPLEMENT_PROPERTIES()
    };
}
