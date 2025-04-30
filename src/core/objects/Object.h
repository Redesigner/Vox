#pragma once
#include <vector>

#include "core/objects/Property.h"

namespace Vox
{
    class Object
    {
    public:
        Object();
        virtual ~Object() = default;

        void InitProperties();
        
        std::vector<Property> GetProperties() const;

    protected:
        virtual void BuildProperties(std::vector<Property>& properties) = 0;
        
    private:
        std::vector<Property> properties;
    };
}
