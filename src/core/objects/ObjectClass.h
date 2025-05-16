#pragma once
#include <functional>

#include "Property.h"

namespace Vox
{
    class Object;

    class ObjectClass
    {
    public:
        ObjectClass(std::function<Object*()> constructor, std::vector<Property> properties);

        [[nodiscard]] std::function<Object*()> GetConstructor() const;
        [[nodiscard]] const std::vector<Property>& GetProperties() const;
        [[nodiscard]] Property* GetPropertyByType(PropertyType type) const;
        
    private:
        std::function<Object*()> constructor;
        std::vector<Property> properties;
    };
}
