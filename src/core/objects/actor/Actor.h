#pragma once
#include "core/objects/Tickable.h"
#include "core/objects/Object.h"
#include "core/objects/component/Component.h"
#include "game_objects/components/SceneComponent.h"


namespace  Vox
{
    class Component;
    class World;

    class Actor : public Object, public Tickable
    {
    public:
        explicit Actor(World* world);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;

        std::weak_ptr<Component> GetWeakChild(const Component* component) const;
        std::weak_ptr<Component> GetWeakChild(const SceneComponent* component) const;

        [[nodiscard]] const std::vector<std::shared_ptr<Component>>& GetComponents() const;
        
        [[nodiscard]] const std::vector<std::shared_ptr<SceneComponent>>& GetAttachedComponents() const;

        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);
        void SetTransform(const Transform& transformIn);

        void Tick(float deltaTime) override;

        [[nodiscard]] World* GetWorld() const;

#ifdef EDITOR
        void Select() const;
#endif

        [[nodiscard]] const Transform& GetTransform() const;

    protected:
        template <typename T, typename... Args>
        std::shared_ptr<T> RegisterComponent(Args&&... args) requires Derived<T, Component> && !Derived<T, SceneComponent>
        {
            auto result = std::static_pointer_cast<T>(components.emplace_back(Component::Create<T>(this, std::forward<Args>(args)...)));
            if (auto tickable = std::dynamic_pointer_cast<Tickable>(result))
            {
                tickableComponents.emplace_back(std::move(tickable));
            }
            return result;
        }

        template <typename T, typename... Args>
        std::shared_ptr<T> AttachComponent(Args&&... args) requires Derived<T, SceneComponent>
        {
            auto result = std::static_pointer_cast<T>(attachedComponents.emplace_back(Component::Create<T>(this, std::forward<Args>(args)...)));
            if (auto tickable = std::dynamic_pointer_cast<Tickable>(result))
            {
                tickableComponents.emplace_back(std::move(tickable));
            }
            return result;
        }
    
    private:
        void UpdateChildTransforms() const;
        
        Transform transform;

        std::vector<std::shared_ptr<Component>> components;

        std::vector<std::shared_ptr<SceneComponent>> attachedComponents;

        std::vector<std::shared_ptr<Tickable>> tickableComponents;

        World* world;

        IMPLEMENT_OBJECT(Actor)
    };
}
