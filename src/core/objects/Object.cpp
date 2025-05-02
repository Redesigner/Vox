#include "Object.h"

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

    const ObjectClass* Object::GetClass() const
    {
        return ServiceLocator::GetObjectService()->GetObjectClass(GetClassDisplayName());
    }

    const std::string& Object::GetDisplayName() const
    {
        return displayName;
    }
}
