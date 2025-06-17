#include "World.h"

#include <nlohmann/json.hpp>

#include "core/logging/Logging.h"
#include "core/math/Strings.h"
#include "core/objects/Tickable.h"
#include "core/services/FileIOService.h"
#include "core/services/InputService.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"
#include "physics/PhysicsServer.h"
#include "rendering/SceneRenderer.h"
#include "voxel/VoxelWorld.h"

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
            auto objectInitializer = ObjectInitializer(this);
            objectInitializer.rootObject = true;
            const auto& constructor = objectClass->GetConstructor();
            auto result = objects.emplace_back(constructor(objectInitializer));
            assert(result);
            PostObjectConstruct(result);
            return result;
        }
        return nullptr;
    }

    std::shared_ptr<Object> World::CreateObject(const ObjectClass* objectClass)
    {
        auto objectInitializer = ObjectInitializer(this);
        objectInitializer.rootObject = true;
        auto result = objects.emplace_back(objectClass->GetConstructor()(objectInitializer));
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

    std::shared_ptr<PhysicsServer> World::GetPhysicsServer() const
    {
        return physicsServer;
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
            else if (state == Paused)
            {
                Unpause();
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

        ServiceLocator::GetFileIoService()->WriteToFile("worlds/" + filename + ".world", Save().Serialize().dump(4));
    }

    void World::Reload()
    {
        SavedWorld save = Save();
        SaveToFile("MainWorld");
        Load(save);
    }

    SavedWorld World::Save() const
    {
        SavedWorld result;

        for (const std::shared_ptr<Object>& child : objects)
        {
            auto tempPrefabContext = PrefabContext(child.get(), child->GetClass());
            result.savedObjects.emplace_back(child->GetDisplayName(), child->GetClass()->GetName(), std::move(tempPrefabContext.propertyOverrides));
        }
        return result;
    }

    void World::Load(const SavedWorld& savedWorld)
    {
        objects.clear();
        actorsToTick.clear();

        for (const SavedWorldObject& object : savedWorld.savedObjects)
        {
            auto objectInitializer = ObjectInitializer(this);
            objectInitializer.rootObject = true;
            const auto objectClass = ServiceLocator::GetObjectService()->GetObjectClass(object.className);
            const std::shared_ptr<Object> child = objects.emplace_back(objectClass->GetConstructor()(objectInitializer));

            for (const auto& propertyOverride : object.worldContextOverrides)
            {
                std::shared_ptr<Object> currentObject = child;
                for (const auto& objectName : propertyOverride.path)
                {
                    currentObject = currentObject->GetChildByName(objectName);
                    if (!currentObject)
                    {
                        VoxLog(Warning, Game, "World could not override property. Object '{}' was not found.", objectName);
                        return;
                    }
                }

                Property* property = currentObject->GetClass()->GetPropertyByName(propertyOverride.propertyName);
                if (!property)
                {
                    VoxLog(Warning, Game, "World could not override property. Property '{}' was not a member of '{}'.", propertyOverride.propertyName, currentObject->GetDisplayName());
                    return;
                }

                property->SetValue(currentObject.get(), propertyOverride.variant.type, propertyOverride.variant.value);
                currentObject->PropertyChanged(*property);
            }
            child->SetName(object.name);
            child->native = false;
            child->PostConstruct();
            PostObjectConstruct(child);
        }
    }

    void World::InitializeVoxels()
    {
        voxels = std::make_unique<VoxelWorld>(this);

        Voxel defaultVoxel;
        defaultVoxel.materialId = 1;
        for (int x = -16; x < 16; ++x)
        {
            for (int y = -16; y < 0; ++y)
            {
                for (int z = -16; z < 16; ++z)
                {
                    voxels->SetVoxel(glm::uvec3(x + 16, y + 16, z + 16), defaultVoxel);
                }
            }
        }
        Voxel testVoxel2;
        testVoxel2.materialId = 2;

        voxels->SetVoxel({-1, 16, -1}, testVoxel2);
        voxels->SetVoxel({0, 16, 0}, testVoxel2);
        voxels->FinalizeUpdate();
    }

    World::World()
    {
        renderer = std::make_shared<SceneRenderer>(this);
        physicsServer = std::make_shared<PhysicsServer>();

        toggleDebugRenderHandle = ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_F3, [this](const bool buttonPressed)
        {
            if (buttonPressed)
            {
                renderer->renderDebug = !renderer->renderDebug;
            }
        });

        togglePauseHandle = ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_PAUSE, [this](const bool buttonPressed)
        {
            if (!buttonPressed)
            {
                return;
            }

            if (state == Playing)
            {
                SetWorldState(Paused);
            }
            else if (state == Paused)
            {
                SetWorldState(Playing);
            }
        });
    }

    World::~World() // NOLINT(*-use-equals-default)
    {
        ServiceLocator::GetInputService()->UnregisterKeyboardCallback(SDL_SCANCODE_F3, toggleDebugRenderHandle);
        ServiceLocator::GetInputService()->UnregisterKeyboardCallback(SDL_SCANCODE_PAUSE, togglePauseHandle);
    }

    void World::PostObjectConstruct(const std::shared_ptr<Object>& object)
    {
        CheckObjectName(object);
        if (const auto tickable = dynamic_cast<Tickable*>(object.get()))
        {
            RegisterTickable(tickable);
        }
        // Objects are never native to a world!
        object->native = false;
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
        initialState = Save();
        physicsServer->running = true;
        for (Tickable* tickable : actorsToTick)
        {
            tickable->Play();
        }
    }

    void World::Pause()
    {
        physicsServer->running = false;
    }

    void World::Unpause()
    {
        physicsServer->running = true;
    }

    void World::Restart()
    {
        physicsServer->running = false;
        renderer->ResetCamera();
        Load(initialState);
    }
}
