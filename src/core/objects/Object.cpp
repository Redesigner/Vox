#include "Object.h"

#include <nlohmann/json.hpp>

#include "ObjectClass.h"
#include "core/math/Strings.h"

namespace Vox
{
    Object::Object(const ObjectInitializer& objectInitializer)
    {
    }

    void Object::PostConstruct()
    {
    }

    const std::vector<Property>& Object::GetProperties() const
    {
        const std::shared_ptr<ObjectClass> objectClass = GetClass();
        assert(objectClass && "ObjectClass was invalid");

        // ReSharper disable once CppDFANullDereference
        return objectClass->GetProperties();
    }

    const std::string& Object::GetDisplayName() const
    {
        return displayName;
    }

    void Object::SetName(const std::string& name)
    {
        displayName = name;
    }

    // May need to create some kind of stack limit here
    nlohmann::ordered_json Object::Serialize(const Object* defaultObject) // NOLINT(*-no-recursion)
    {
        using json = nlohmann::ordered_json;

        if (!defaultObject)
        {
            defaultObject = GetClass()->GetDefaultObject();
        }

        json objectJson {};
        objectJson[displayName]["class"] = GetClassDisplayName();

        objectJson[displayName]["properties"] = json::value_type::object();
        for (const Property& property : GetProperties())
        {
            if (property.ValueEquals(defaultObject, this))
            {
                continue;
            }

            json propertyJson = property.Serialize(this);
            objectJson[displayName]["properties"].insert(propertyJson.begin(), propertyJson.end());
        }

        if (objectJson[displayName]["properties"].empty())
        {
            objectJson.erase(displayName);
            return objectJson;
        }

        return objectJson;
    }

    std::vector<PropertyOverride> Object::GenerateOverrides(const ObjectClass* defaultClass) const
    {
        std::vector<std::string> context {};
        const Object* defaultObject = defaultClass ? defaultClass->GetDefaultObject() : GetClass()->GetDefaultObject();
        return GenerateOverrides(defaultObject, context);
    }

    std::vector<PropertyOverride> Object::GenerateOverrides(const Object* defaultObject,
        std::vector<std::string>& context) const
    {
        std::vector<PropertyOverride> result;
        for (const Property& property : GetProperties())
        {
            if (property.ValueEquals(defaultObject, this))
            {
                continue;
            }

            result.emplace_back(context, property.GetName(), property.GetTypedVariant(this));
        }

        return result;
    }
}
