#pragma once

#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    class World
    {
    public:
        [[nodiscard]] const std::vector<Object*>& GetObjects() const;

        template <typename T, class... Args>
        Object* CreateObject(Args&&... args) requires Derived<T, Object>
        {
            return objects.emplace_back( new T(std::forward<Args>(args)...));
        }

        Object* CreateObject(const std::string& className);

        ~World();
        
    private:
        //@TODO: proper heap allocator here
        std::vector<Object*> objects;
    };
}
