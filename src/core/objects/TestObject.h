#pragma once

#include "core/objects/Object.h"
#include "core/objects/Property.h"

namespace Vox
{
    class TestObject : public Object
    {
    protected:
        void BuildProperties(std::vector<Property>& properties) override;
        
    private:
        float test = 0.0f;
        
    };
}
