#pragma once

#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"

namespace Vox
{
    class Actor;

    class Component : public Object
    {
    public:
        explicit Component(Actor* parent);

        [[nodiscard]] Actor* GetParent() const;

        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
        template <class T, typename... Args>
        static std::shared_ptr<T> Create(Actor* parent, Args&&... args) requires Derived<T, Component>
        {
            std::shared_ptr<T> result = std::make_shared<T>(parent, std::forward<Args>(args)...);
            return result;
        }

    protected:
        [[nodiscard]] virtual std::weak_ptr<Component> GetWeakThis() const;

    private:
        Actor* parent = nullptr;
        
        IMPLEMENT_OBJECT(Component)
    };
}
