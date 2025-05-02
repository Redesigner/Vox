#include "ObjectService.h"

#include <cassert>

namespace Vox
{
    void ObjectService::RegisterObjectClass(const std::string& classId, const ObjectClass& objectClass)
    {
        assert(!classRegistry.contains(classId) && "Class is already registered");
        classRegistry.insert({classId, objectClass});
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
