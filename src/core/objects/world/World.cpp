#include "World.h"

#include <nlohmann/json.hpp>

#include "../../../game_objects/actors/Actor.h"
#include "core/logging/Logging.h"
#include "core/math/Strings.h"
#include "../interfaces/Tickable.h"
#include "core/services/FileIOService.h"
#include "core/services/InputService.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"
#include "physics/PhysicsServer.h"
#include "rendering/SceneRenderer.h"
#include "voxel/VoxelWorld.h"

namespace Vox
{
    const std::vector<std::shared_ptr<Actor>>& World::GetActors() const
    {
        return actors;
    }

    std::shared_ptr<Actor> World::CreateActor(const std::string& className)
    {
        if (const std::shared_ptr<ObjectClass> objectClass = ServiceLocator::GetObjectService()->GetObjectClass(className))
        {
            auto objectInitializer = ObjectInitializer(this);
            objectInitializer.rootObject = true;
            const auto& constructor = objectClass->GetConstructor();
            // @TODO: Implement actor class or other method to avoid dynamic casts here
            auto result = actors.emplace_back(std::dynamic_pointer_cast<Actor>(constructor(objectInitializer)));
            assert(result);
            PostActorConstruct(result);
            return result;
        }
        return nullptr;
    }

    std::shared_ptr<Actor> World::CreateActor(const ObjectClass* objectClass)
    {
        auto objectInitializer = ObjectInitializer(this);
        objectInitializer.rootObject = true;
        auto result = actors.emplace_back(std::dynamic_pointer_cast<Actor>(objectClass->GetConstructor()(objectInitializer)));
        PostActorConstruct(result);
        return result;
    }

    void World::Tick(const float deltaTime)
    {
        if (state != WorldState::Playing)
        {
            return;
        }

        tickManager.Tick(deltaTime);
    }

    void World::DestroyActor(const std::shared_ptr<Actor>& actor)
    {
        std::erase_if(actors, [actor](const std::shared_ptr<Object>& ownedObject)
        {
           return ownedObject == actor;
        });
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
        case WorldState::Playing:
            if (state == WorldState::Inactive)
            {
                Play();
            }
            else if (state == WorldState::Paused)
            {
                Unpause();
            }
            break;
        case WorldState::Paused:
            Pause();
            break;
        case WorldState::Inactive:
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

        if (voxels)
        {
            voxels->SaveToFile(filename);
        }
    }

    void World::Reload()
    {
        const SavedWorld save = Save();
        SaveToFile("MainWorld");
        Load(save);
    }

    TickManager& World::GetTickManager()
    {
        return tickManager;
    }

    SavedWorld World::Save() const
    {
        SavedWorld result;

        for (const std::shared_ptr<Actor>& child : actors)
        {
            auto tempPrefabContext = PrefabContext(child.get(), child->GetClass());
            result.savedObjects.emplace_back(child->GetDisplayName(), child->GetClass()->GetName(), std::move(tempPrefabContext.propertyOverrides));
        }
        return result;
    }

    void World::Load(const SavedWorld& savedWorld)
    {
        actors.clear();

        for (const SavedWorldObject& object : savedWorld.savedObjects)
        {
            auto objectInitializer = ObjectInitializer(this);
            objectInitializer.rootObject = true;
            const auto objectClass = ServiceLocator::GetObjectService()->GetObjectClass(object.className);
            const std::shared_ptr<Actor> child = actors.emplace_back(std::dynamic_pointer_cast<Actor>(objectClass->GetConstructor()(objectInitializer)));

            for (const auto& propertyOverride : object.worldContextOverrides)
            {
                std::shared_ptr<Component> childComponent = child->GetChildByName(propertyOverride.path.front());

                Property* property = childComponent->GetClass()->GetPropertyByName(propertyOverride.propertyName);
                if (!property)
                {
                    VoxLog(Warning, Game, "World could not override property. Property '{}' was not a member of '{}'.", propertyOverride.propertyName, childComponent->GetDisplayName());
                    return;
                }

                property->SetValue(childComponent.get(), propertyOverride.variant.type, propertyOverride.variant.value);
                childComponent->PropertyChanged(*property);
            }
            child->SetName(object.name);
            child->native = false;
            child->PostConstruct();
            PostActorConstruct(child);
        }
    }

    void World::LoadVoxels(const std::string& filename)
    {
        voxels = std::make_unique<VoxelWorld>(this, filename);
    }

    void World::InitializeVoxels()
    {
        // voxels = std::make_unique<VoxelWorld>(this);
        //
        // Voxel defaultVoxel;
        // defaultVoxel.materialId = 1;
        // for (int x = -16; x < 16; ++x)
        // {
        //     for (int y = -16; y < 0; ++y)
        //     {
        //         for (int z = -16; z < 16; ++z)
        //         {
        //             voxels->SetVoxel(glm::uvec3(x + 16, y + 16, z + 16), defaultVoxel);
        //         }
        //     }
        // }
        // voxels->FinalizeUpdate();
    }

    VoxelWorld* World::GetVoxels() const
    {
        return voxels.get();
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

            if (state == WorldState::Playing)
            {
                SetWorldState(WorldState::Paused);
            }
            else if (state == WorldState::Paused)
            {
                SetWorldState(WorldState::Playing);
            }
        });
    }

    World::~World() // NOLINT(*-use-equals-default)
    {
        ServiceLocator::GetInputService()->UnregisterKeyboardCallback(SDL_SCANCODE_F3, toggleDebugRenderHandle);
        ServiceLocator::GetInputService()->UnregisterKeyboardCallback(SDL_SCANCODE_PAUSE, togglePauseHandle);
    }

    void World::PostActorConstruct(const std::shared_ptr<Actor>& actor)
    {
        CheckActorName(actor);
        if (const auto tickable = std::dynamic_pointer_cast<Tickable>(actor))
        {
            tickManager.RegisterTickable(tickable);
        }

        // Objects are never native to a world!
        actor->native = false;
    }

    void World::CheckActorName(const std::shared_ptr<Actor>& actor) const
    {
        std::string objectName = actor->GetDisplayName();
        while (FindActor(actor, objectName))
        {
            objectName = IncrementString(objectName);
        }
        actor->SetName(objectName);
    }

    std::shared_ptr<Actor> World::FindActor(const std::string& name) const
    {
        auto result = std::ranges::find_if(actors, [name](const std::shared_ptr<Actor>& child)
        {
            return name == child->GetDisplayName();
        });
        if (result == actors.end())
        {
            return nullptr;
        }
        return *result;
    }

    std::shared_ptr<Actor> World::FindActor(const std::shared_ptr<Actor>& exclude,
        const std::string& name) const
    {
        auto result = std::ranges::find_if(actors, [exclude, name](const std::shared_ptr<Actor>& child)
        {
            return child != exclude && name == child->GetDisplayName();
        });
        if (result == actors.end())
        {
            return nullptr;
        }
        return *result;
    }

    void World::Play()
    {
        initialState = Save();
        physicsServer->running = true;
        tickManager.Play();
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
