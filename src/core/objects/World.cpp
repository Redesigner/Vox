#include "World.h"

#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    const std::vector<Object*>& World::GetObjects() const
    {
        return objects;
    }

    Object* World::CreateObject(const std::string& className)
    {
        if (const ObjectClass* objectClass = ServiceLocator::GetObjectService()->GetObjectClass(className))
        {
            return objects.emplace_back(objectClass->GetConstructor()());
        }
        return nullptr;
    }

    World::~World()
    {
        for (Object* object : objects)
        {
            delete object;
        }
        objects.clear();
    }
}
