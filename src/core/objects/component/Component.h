#pragma once
#include "core/objects/Object.h"

namespace Vox
{
    class Actor;

    class Component : public Object
    {
    public:
        Actor* GetParent() const;

        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
        template <class T, typename... Args>
        static std::unique_ptr<T> Create(Actor* parent, Args&&... args)
        {
            static_assert(std::is_convertible<T*, Component*>());
            std::unique_ptr<T> result = std::make_unique<T>(std::forward<Args>(args)...);
            result->parent = parent;
            return result;
        }
        
    private:
        Actor* parent = nullptr;
        
        IMPLEMENT_NAME(Component)
        IMPLEMENT_PROPERTIES()
    };
}
