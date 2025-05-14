#pragma once

#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    class Actor;
    class Tickable;

    class World
    {
    public:
        [[nodiscard]] const std::vector<Object*>& GetObjects() const;

        template <typename T, class... Args>
        T* CreateObject(Args&&... args) requires Derived<T, Object>
        {
            T* newObject = static_cast<T*>(objects.emplace_back( new T(std::forward<Args>(args)...)));
            InsertCheckTickable(newObject);
            return newObject;
        }

        Object* CreateObject(const std::string& className);

        void Tick(float deltaTime);

        ~World();
        
    private:
        void InsertCheckTickable(Object* object);

        void RegisterTickable(Tickable* tickable);

        //@TODO: proper heap allocator here
        std::vector<Object*> objects;
        std::vector<Tickable*> actorsToTick;
    };
}
