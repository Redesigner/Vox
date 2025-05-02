#include "ObjectClass.h"

namespace Vox
{
    ObjectClass::ObjectClass(std::function<Object*()> constructor, std::vector<Property> properties)
        :constructor(std::move(constructor)), properties(std::move(properties))
    {
    }

    std::function<Object*()> ObjectClass::GetConstructor() const
    {
        return constructor;
    }

    const std::vector<Property>& ObjectClass::GetProperties() const
    {
        return properties;
    }
}
