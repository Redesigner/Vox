#include "World.h"

#include "core/objects/Tickable.h"
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

    void World::Tick(float deltaTime)
    {
        for (Tickable* tickable : actorsToTick)
        {
            tickable->Tick(deltaTime);
        }
    }

    World::~World()
    {
        for (Object* object : objects)
        {
            delete object;
        }
        objects.clear();
    }

    void World::InsertCheckTickable(Object* object)
    {
        if (const auto tickable = dynamic_cast<Tickable*>(object))
        {
            RegisterTickable(tickable);
        }
    }

    void World::RegisterTickable(Tickable* tickable)
    {
        actorsToTick.push_back(tickable);
    }
}
