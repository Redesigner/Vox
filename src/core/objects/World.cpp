#include "World.h"

#include <nlohmann/json.hpp>

#include "core/math/Strings.h"
#include "core/objects/Tickable.h"
#include "core/services/FileIOService.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    const std::vector<std::shared_ptr<Object>>& World::GetObjects() const
    {
        return objects;
    }

    std::shared_ptr<Object> World::CreateObject(const std::string& className)
    {
        if (const std::shared_ptr<ObjectClass> objectClass = ServiceLocator::GetObjectService()->GetObjectClass(className))
        {
            const auto& constructor = objectClass->GetConstructor();
            auto result = objects.emplace_back(constructor(ObjectInitializer(this)));
            assert(result);
            PostObjectConstruct(result);
            return result;
        }
        return nullptr;
    }

    std::shared_ptr<Object> World::CreateObject(const ObjectClass* objectClass)
    {
        auto result = objects.emplace_back(objectClass->GetConstructor()(ObjectInitializer(this)));
        PostObjectConstruct(result);
        return result;
    }

    void World::Tick(float deltaTime)
    {
        if (state != Playing)
        {
            return;
        }

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

    std::shared_ptr<SceneRenderer> World::GetRenderer() const
    {
        return renderer;
    }

    void World::SetWorldState(WorldState worldState)
    {
        if (worldState == state)
        {
            return;
        }

        switch (worldState)
        {
        case Playing:
            if (state == Inactive)
            {
                Play();
            }
            break;
        case Paused:
            Pause();
            break;
        case Inactive:
            Restart();
            break;
        }

        state = worldState;
    }

    WorldState World::GetWorldState() const
    {
        return state;
    }

    void World::SaveToFile(const std::string& filename)
    {
        if (state != WorldState::Inactive)
        {
            return;
        }

        using Json = nlohmann::ordered_json;

        Json worldJson;
        worldJson["objects"] = Json::object();
        for (const auto& child : objects)
        {
            const Json& childJson = child->Serialize();
            if (childJson.empty())
            {
                worldJson["objects"][child->GetDisplayName()] = Json::object();
            }
            else
            {
                worldJson["objects"].insert(childJson.begin(), childJson.end());
            }
        }

        ServiceLocator::GetFileIoService()->WriteToFile("worlds/" + filename + ".world", worldJson.dump(4));
    }

    void World::Reload()
    {
        SavedWorld save = Save();
        Load(save);
    }

    SavedWorld World::Save() const
    {
        SavedWorld result;

        for (const std::shared_ptr<Object>& child : objects)
        {
            result.savedObjects.emplace_back(child->GetDisplayName(), Prefab(child.get()));
        }
        return result;
    }

    void World::Load(const SavedWorld& savedWorld)
    {
        objects.clear();
        actorsToTick.clear();

        for (const SavedWorldObject& object : savedWorld.savedObjects)
        {
            const std::shared_ptr<Object> child = objects.emplace_back(object.prefab.GetConstructor()(ObjectInitializer(this)));
            child->SetName(object.name);
            PostObjectConstruct(child);
        }
    }

    World::World()
    {
        renderer = std::make_shared<SceneRenderer>();
    }

    World::~World()
    = default;

    void World::PostObjectConstruct(const std::shared_ptr<Object>& object)
    {
        CheckObjectName(object);
        if (const auto tickable = dynamic_cast<Tickable*>(object.get()))
        {
            RegisterTickable(tickable);
        }
    }

    void World::RegisterTickable(Tickable* tickable)
    {
        actorsToTick.push_back(tickable);
    }

    void World::CheckObjectName(const std::shared_ptr<Object>& object) const
    {
        std::string objectName = object->GetDisplayName();
        while (FindObject(object, objectName))
        {
            objectName = IncrementString(objectName);
        }
        object->SetName(objectName);
    }

    std::shared_ptr<Object> World::FindObject(const std::string& name) const
    {
        auto result = std::ranges::find_if(objects, [name](const std::shared_ptr<Object>& child)
        {
            return name == child->GetDisplayName();
        });
        if (result == objects.end())
        {
            return nullptr;
        }
        return *result;
    }

    std::shared_ptr<Object> World::FindObject(const std::shared_ptr<Object>& exclude,
        const std::string& name) const
    {
        auto result = std::ranges::find_if(objects, [exclude, name](const std::shared_ptr<Object>& child)
        {
            return child != exclude && name == child->GetDisplayName();
        });
        if (result == objects.end())
        {
            return nullptr;
        }
        return *result;
    }

    void World::Play()
    {
        for (Tickable* tickable : actorsToTick)
        {
            tickable->Play();
        }
    }

    void World::Pause()
    {
    }

    void World::Restart()
    {
    }
}
