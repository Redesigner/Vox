#pragma once
#include "core/objects/Object.h"
#include "core/objects/component/Component.h"
#include "game_objects/components/SceneComponent.h"


namespace  Vox
{
    class Component;

    class Actor : public Object
    {
    public:
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;

        [[nodiscard]] const std::vector<std::shared_ptr<Component>>& GetComponents() const;
        
        [[nodiscard]] const std::vector<std::shared_ptr<SceneComponent>>& GetAttachedComponents() const;

        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);
        void SetTransform(const Transform& transformIn);

        [[nodiscard]] const Transform& GetTransform() const;
        
    protected:
        template <typename T, typename... Args>
        std::weak_ptr<T> RegisterComponent(Args&&... args) requires Derived<T, Component> && !Derived<T, SceneComponent>
        {
            return std::static_pointer_cast<T>(components.emplace_back(Component::Create<T>(this, std::forward<Args>(args)...)));
        }

        template <typename T, typename... Args>
        std::weak_ptr<T> AttachComponent(Args&&... args) requires Derived<T, SceneComponent>
        {
            return std::static_pointer_cast<T>(attachedComponents.emplace_back(Component::Create<T>(this, std::forward<Args>(args)...)));
        }
    
    private:
        void UpdateChildTransforms() const;
        
        Transform transform;

        std::vector<std::shared_ptr<Component>> components;

        std::vector<std::shared_ptr<SceneComponent>> attachedComponents;
        
        IMPLEMENT_OBJECT(Actor)
    };
}
