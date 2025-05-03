#include "Object.h"

#include <nlohmann/json.hpp>

#include "TestObject.h"
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

    nlohmann::ordered_json Object::Serialize() const
    {
        using json = nlohmann::ordered_json;

        json objectJson {};

        objectJson["objectName"] = GetDisplayName();
    }
}
