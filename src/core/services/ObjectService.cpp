#include "ObjectService.h"

#include <cassert>

namespace Vox
{
    const ObjectClass* ObjectService::RegisterObjectClass(const std::string& classId, const ObjectClass& objectClass)
    {
        assert(!classRegistry.contains(classId) && "Class is already registered");
        auto newClassEntry = classRegistry.insert({classId, objectClass});
        return &newClassEntry.first->second;
    }

    const ObjectClass* ObjectService::GetObjectClass(const std::string& objectClassId) const
    {
        auto iterator = classRegistry.find(objectClassId);
        if (iterator == classRegistry.end())
        {
            return nullptr;
        }
        return &iterator->second;
    }
}
