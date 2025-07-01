#pragma once

#include "core/objects/world/SavedWorld.h"
#include "core/objects/world/TickManager.h"
#include "core/concepts/Concepts.h"
#include "core/datatypes/DelegateHandle.h"
#include "core/objects/Object.h"

namespace Vox
{
    class VoxelWorld;
    class PhysicsServer;
    class Prefab;
    class SceneRenderer;
}

namespace Vox
{
    class Actor;
    class Tickable;

    enum WorldState : char
    {
        Inactive,
        Playing,
        Paused
    };

    class World
    {
    public:
        World();
        ~World();

        World(const World&&) = delete;
        World& operator=(World&&) = delete;

        [[nodiscard]] const std::vector<std::shared_ptr<Actor>>& GetActors() const;

        template <typename T, class... Args>
        std::shared_ptr<T> CreateActor(Args&&... args) requires Derived<T, Actor>
        {
            auto objectInitializer = ObjectInitializer(this);
            objectInitializer.rootObject = true;
            std::shared_ptr<T> newActor = std::static_pointer_cast<T>(actors.emplace_back(
                std::make_shared<T>(objectInitializer, std::forward<Args>(args)...))
            );
            PostObjectConstruct(newActor);
            return newActor;
        }

        std::shared_ptr<Actor> CreateActor(const std::string& className);

        std::shared_ptr<Actor> CreateActor(const ObjectClass* objectClass);

        void Tick(float deltaTime);

        void DestroyActor(const std::shared_ptr<Actor>& actor);

        [[nodiscard]] std::shared_ptr<SceneRenderer> GetRenderer() const;

        [[nodiscard]] std::shared_ptr<PhysicsServer> GetPhysicsServer() const;

        void SetWorldState(WorldState worldState);

        [[nodiscard]] WorldState GetWorldState() const;

        void SaveToFile(const std::string& filename);

        void Reload();

        [[nodiscard]] TickManager& GetTickManager();

        [[nodiscard]] SavedWorld Save() const;

        void Load(const SavedWorld& savedWorld);

        void LoadVoxels(const std::string& filename);

        void InitializeVoxels();

        [[nodiscard]] VoxelWorld* GetVoxels() const;

    private:
        void PostActorConstruct(const std::shared_ptr<Actor>& actor);

        void CheckActorName(const std::shared_ptr<Actor>& actor) const;

        [[nodiscard]] std::shared_ptr<Actor> FindActor(const std::string& name) const;
        [[nodiscard]] std::shared_ptr<Actor> FindActor(const std::shared_ptr<Actor>& exclude, const std::string& name) const;

        void Play();

        void Pause();

        void Unpause();

        void Restart();

        WorldState state = WorldState::Inactive;

        std::shared_ptr<SceneRenderer> renderer;

        std::shared_ptr<PhysicsServer> physicsServer;

        std::unique_ptr<VoxelWorld> voxels;

        //@TODO: proper heap allocator here
        std::vector<std::shared_ptr<Actor>> actors;

        DelegateHandle<bool> toggleDebugRenderHandle;
        DelegateHandle<bool> togglePauseHandle;

        SavedWorld initialState;

        TickManager tickManager;
    };
}
