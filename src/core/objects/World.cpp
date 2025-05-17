#include "World.h"

#include "core/objects/Tickable.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    const std::vector<std::shared_ptr<Object>>& World::GetObjects() const
    {
        return objects;
    }

    std::shared_ptr<Object> World::CreateObject(const std::string& className)
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

    void World::DestroyObject(const std::shared_ptr<Object>& object)
    {
        std::erase_if(objects, [object](const std::shared_ptr<Object>& ownedObject)
        {
           return ownedObject == object;
        });

        if (auto* tickable = dynamic_cast<Tickable*>(object.get()))
        {
            std::erase_if(actorsToTick, [tickable](const Tickable* tickableObject)
            {
               return tickable == tickableObject;
            });
        }
    }

    World::~World()
    = default;

    void World::InsertCheckTickable(const std::shared_ptr<Object>& object)
    {
        if (const auto tickable = dynamic_cast<Tickable*>(object.get()))
        {
            RegisterTickable(tickable);
        }
    }

    void World::RegisterTickable(Tickable* tickable)
    {
        actorsToTick.push_back(tickable);
    }
}
