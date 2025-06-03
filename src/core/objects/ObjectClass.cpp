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

    ObjectClass* ObjectClass::GetParentClass() const
    {
        return parentClass;
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

    Property* ObjectClass::GetPropertyByName(const std::string& name) const
    {
        const auto result = std::ranges::find_if(properties, [name](const Property& property)
        {
            return property.GetName() == name;
        });

        if (result == properties.end())
        {
            return nullptr;
        }

        return result._Ptr;
    }

#ifdef EDITOR
    bool ObjectClass::CanBeRenamed() const
    {
        return canBeRenamed;
    }
#endif
}
