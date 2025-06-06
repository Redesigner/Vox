#include "ObjectService.h"

#include <cassert>

namespace Vox
{
    std::shared_ptr<ObjectClass> ObjectService::RegisterObjectClass(const std::string& classId, const std::shared_ptr<ObjectClass>& objectClass)
    {
        assert(!classRegistry.contains(classId) && "Class is already registered");
        auto newClassEntry = classRegistry.insert({classId, objectClass});
        return newClassEntry.first->second;
    }

    void ObjectService::RegisterPrefab(const std::string& filename)
    {
        classRegistry.emplace(filename, std::make_shared<Prefab>(filename));
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
