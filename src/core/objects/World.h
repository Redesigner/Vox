#pragma once

#include "core/objects/Object.h"

namespace Vox
{
    class World
    {
    public:
        [[nodiscard]] const std::vector<Object*>& GetObjects() const;

        template <typename T, class... Args>
        Object* CreateObject(Args&&... args)
        {
            static_assert(std::is_base_of_v<Object, T>);

            return objects.emplace_back( new T(std::forward<Args>(args)...));
        }

        ~World();
        
    private:
        //@TODO: proper heap allocator here
        std::vector<Object*> objects;
    };
}
