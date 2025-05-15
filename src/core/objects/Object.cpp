#include "Object.h"

#include <nlohmann/json.hpp>

#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    const std::vector<Property>& Object::GetProperties() const
    {
        const ObjectClass* objectClass = GetClass();
        assert(objectClass && "ObjectClass was invalid");
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

    nlohmann::ordered_json Object::Serialize()
    {
        using json = nlohmann::ordered_json;

        json objectJson {};
        objectJson[displayName]["class"] = GetClassDisplayName();

        for (const Property& property : GetProperties())
        {
            json propertyJson = property.Serialize(this);
            objectJson[displayName].insert(propertyJson.begin(), propertyJson.end());
        }
        return objectJson;
    }
}
