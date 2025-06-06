#include "ObjectService.h"

#include <cassert>

namespace Vox
{
    void ObjectService::RegisterPrefab(const std::string& filename)
    {
        classRegistry.emplace(filename, Prefab::FromFile(filename));
    }

    std::shared_ptr<ObjectClass> ObjectService::GetObjectClass(const std::string& objectClassId) const
    {
        auto iterator = classRegistry.find(objectClassId);
        if (iterator == classRegistry.end())
        {
            return nullptr;
        }
        return iterator->second;
    }

    std::map<std::string, std::shared_ptr<ObjectClass>>::const_iterator ObjectService::GetBegin() const
    {
        return classRegistry.cbegin();
    }

    std::map<std::string, std::shared_ptr<ObjectClass>>::const_iterator ObjectService::GetEnd() const
    {
        return classRegistry.cend();
    }

    DelegateHandle<const ObjectClass*> ObjectService::RegisterClassChangedDelegate(
        const std::function<void(const ObjectClass*)>& delegate)
    {
        return objectClassChanged.RegisterCallback(delegate);
    }

    void ObjectService::UnregisterClassChangedDelegate(DelegateHandle<const ObjectClass*> handle)
    {
        objectClassChanged.UnregisterCallback(handle);
    }

    void ObjectService::UpdateClass(const ObjectClass* objectClass)
    {
        objectClassChanged(objectClass);
    }
}
