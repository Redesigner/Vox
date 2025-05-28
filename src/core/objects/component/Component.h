#pragma once

#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"

namespace Vox
{
    class Actor;

    class Component : public Object
    {
    public:
        explicit Component(const ObjectInitializer& objectInitializer);

        [[nodiscard]] Actor* GetActor() const;

        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
        template <class T, typename... Args>
        static std::shared_ptr<T> Create(const ObjectInitializer& objectInitializer, Args&&... args) requires Derived<T, Component>
        {
            std::shared_ptr<T> result = std::make_shared<T>(objectInitializer, std::forward<Args>(args)...);
            return result;
        }

    private:
        IMPLEMENT_OBJECT(Component)
    };
}
