#pragma once

#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    class SceneRenderer;
}

namespace Vox
{
    class Actor;
    class Tickable;

    class World
    {
    public:
        World();
        ~World();

        [[nodiscard]] const std::vector<std::shared_ptr<Object>>& GetObjects() const;

        template <typename T, class... Args>
        std::shared_ptr<T> CreateObject(Args&&... args) requires Derived<T, Object>
        {
            std::shared_ptr<T> newObject = std::static_pointer_cast<T>(objects.emplace_back(std::make_shared<T>(this, std::forward<Args>(args)...)));
            InsertCheckTickable(newObject);
            return newObject;
        }

        std::shared_ptr<Object> CreateObject(const std::string& className);

        std::shared_ptr<Object> CreateObject(const ObjectClass* objectClass);

        void Tick(float deltaTime);

        void DestroyObject(const std::shared_ptr<Object>& object);

        [[nodiscard]] std::shared_ptr<SceneRenderer> GetRenderer() const;

    private:
        void InsertCheckTickable(const std::shared_ptr<Object>& object);

        void RegisterTickable(Tickable* tickable);

        std::shared_ptr<SceneRenderer> renderer;

        //@TODO: proper heap allocator here
        std::vector<std::shared_ptr<Object>> objects;
        std::vector<Tickable*> actorsToTick;
    };
}
