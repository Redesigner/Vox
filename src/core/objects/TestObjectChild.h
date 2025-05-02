#pragma once
#include "TestObject.h"

namespace Vox
{
    class ITestInterface
    {
    public:
        virtual void doSomething() = 0;
    };
    
    class TestObjectChild : public TestObject
    {
    public:
        TestObjectChild();
        
        //void doSomething() override {};
        
    protected:
        void BuildProperties(std::vector<Property>& properties) override;

        
    private:
        float test2 = 5.0f;
        std::string testString3 = "this string is a member of TestObjectChild";

        glm::vec3 position = {5.0f, 15.0f, 0.0f};
        
        IMPLEMENT_NAME(TestObjectChild)
        IMPLEMENT_PROPERTIES()
    };
}
