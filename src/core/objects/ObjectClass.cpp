#include "ObjectClass.h"

#include "Object.h"

namespace Vox
{
    using Constructor = std::function<std::shared_ptr<Object>(const ObjectInitializer&)>;

    ObjectClass::ObjectClass(Constructor constructor, std::vector<Property> properties)
        :constructor(std::move(constructor)), properties(std::move(properties))
    {
    }

    Constructor ObjectClass::GetConstructor() const
    {
        return constructor;
    }

    const std::vector<Property>& ObjectClass::GetProperties() const
    {
        return properties;
    }

    Property* ObjectClass::GetPropertyByType(PropertyType type) const
    {
        const auto result = std::ranges::find_if(properties, [type](const Property& property)
        {
            return property.GetType() == type;
        });

        if (result == properties.end())
        {
            return nullptr;
        }

        return result._Ptr;
    }
}
