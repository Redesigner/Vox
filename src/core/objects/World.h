#pragma once

#include "SavedWorld.h"
#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"

namespace Vox
{
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

        [[nodiscard]] const std::vector<std::shared_ptr<Object>>& GetObjects() const;

        template <typename T, class... Args>
        std::shared_ptr<T> CreateObject(Args&&... args) requires Derived<T, Object>
        {
            auto objectInitializer = ObjectInitializer(this);
            objectInitializer.rootObject = true;
            std::shared_ptr<T> newObject = std::static_pointer_cast<T>(objects.emplace_back(
                std::make_shared<T>(objectInitializer, std::forward<Args>(args)...))
            );
            PostObjectConstruct(newObject);
            return newObject;
        }

        std::shared_ptr<Object> CreateObject(const std::string& className);

        std::shared_ptr<Object> CreateObject(const ObjectClass* objectClass);

        void Tick(float deltaTime);

        void DestroyObject(const std::shared_ptr<Object>& object);

        [[nodiscard]] std::shared_ptr<SceneRenderer> GetRenderer() const;

        void SetWorldState(WorldState worldState);

        [[nodiscard]] WorldState GetWorldState() const;

        void SaveToFile(const std::string& filename);

        void Reload();

        [[nodiscard]] SavedWorld Save() const;

        void Load(const SavedWorld& savedWorld);

    private:
        void PostObjectConstruct(const std::shared_ptr<Object>& object);

        void RegisterTickable(Tickable* tickable);

        void CheckObjectName(const std::shared_ptr<Object>& object) const;

        [[nodiscard]] std::shared_ptr<Object> FindObject(const std::string& name) const;
        [[nodiscard]] std::shared_ptr<Object> FindObject(const std::shared_ptr<Object>& exclude, const std::string& name) const;

        void Play();

        void Pause();

        void Restart();

        WorldState state = WorldState::Inactive;

        std::shared_ptr<SceneRenderer> renderer;

        //@TODO: proper heap allocator here
        std::vector<std::shared_ptr<Object>> objects;
        std::vector<Tickable*> actorsToTick;
    };
}
