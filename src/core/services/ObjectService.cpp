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

    void ObjectService::RegisterPrefab(const std::string& filename)
    {
        prefabRegistry.emplace(filename, std::move(Prefab(filename)));
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

    std::unordered_map<std::string, ObjectClass>::const_iterator ObjectService::GetClassBegin() const
    {
        return classRegistry.cbegin();
    }

    std::unordered_map<std::string, ObjectClass>::const_iterator ObjectService::GetClassEnd() const
    {
        return classRegistry.cend();
    }

    const Prefab* ObjectService::GetPrefab(const std::string& prefabId) const
    {
        auto iterator = prefabRegistry.find(prefabId);
        if (iterator == prefabRegistry.end())
        {
            return nullptr;
        }
        return &iterator->second;
    }

    std::unordered_map<std::string, Prefab>::const_iterator ObjectService::GetPrefabBegin() const
    {
        return prefabRegistry.cbegin();
    }

    std::unordered_map<std::string, Prefab>::const_iterator ObjectService::GetPrefabEnd() const
    {
        return prefabRegistry.cend();
    }
}
