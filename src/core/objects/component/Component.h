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

        void BuildProperties(std::vector<Property>& propertiesInOut) override;
        
        template <class T, typename... Args>
        static std::shared_ptr<T> Create(const ObjectInitializer& objectInitializer, Args&&... args) requires Derived<T, Component>
        {
            std::shared_ptr<T> result = std::make_shared<T>(objectInitializer, std::forward<Args>(args)...);
            return result;
        }

        /**
         * @brief Get the parent object of this
         * @return Parent. Can be nullptr
         */
        [[nodiscard]] Actor* GetParent() const;

        void SetParent(Actor* parentIn) const;

        /**
         * @brief Get a shared_ptr to this object from the parent
         * @return shared_ptr to this. Can be nullptr if parent is nullptr
         */
        [[nodiscard]] std::shared_ptr<Component> GetSharedThis() const;

    private:
        Actor* parent;

        IMPLEMENT_OBJECT_BASE(Component)
    };
}
