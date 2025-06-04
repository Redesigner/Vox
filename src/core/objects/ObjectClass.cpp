#include "ObjectClass.h"

#include <utility>

#include "Object.h"

namespace Vox
{
    using Constructor = std::function<std::shared_ptr<Object>(const ObjectInitializer&)>;

    ObjectClass::ObjectClass(std::string  name, Constructor constructor, const std::shared_ptr<ObjectClass>& parent, std::vector<Property> properties)
        : constructor(std::move(constructor)), parentClass(parent), properties(std::move(properties)),
          name(std::move(name))
    {
    }

    Constructor ObjectClass::GetConstructor() const
    {
        return constructor;
    }

    std::shared_ptr<ObjectClass> ObjectClass::GetParentClass() const
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

    const std::string& ObjectClass::GetName() const
    {
        return name;
    }

#ifdef EDITOR
    bool ObjectClass::CanBeRenamed() const
    {
        return canBeRenamed;
    }
#endif

    template <>
    bool ObjectClass::IsA<Object>() const
    { return true; }
}
