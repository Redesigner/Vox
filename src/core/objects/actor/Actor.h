#pragma once
#include "core/objects/Tickable.h"
#include "core/objects/Object.h"
#include "core/objects/ObjectClass.h"
#include "core/objects/component/Component.h"
#include "game_objects/components/SceneComponent.h"


namespace  Vox
{
    class Component;
    class World;

    class Actor : public Object, public Tickable
    {
    public:
        explicit Actor(const ObjectInitializer& objectInitializer);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;
        
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

        template <typename T, typename... Args>
        std::shared_ptr<T> AttachComponent(Args&&... args) requires Derived<T, SceneComponent>
        {
            ObjectInitializer objectInitializer;
            objectInitializer.world = world;
            objectInitializer.parent = this;
            auto result = std::static_pointer_cast<T>(attachedComponents.emplace_back(Component::Create<T>(objectInitializer, std::forward<Args>(args)...)));
            AddChild(result);
            if (auto tickable = std::dynamic_pointer_cast<Tickable>(result))
            {
                tickableComponents.emplace_back(std::move(tickable));
            }
            return result;
        }

        template <typename... Args>
        std::shared_ptr<SceneComponent> AttachComponent(const ObjectClass* componentClass, Args&&... args)
        {
            assert(componentClass->IsA<SceneComponent>());

            ObjectInitializer objectInitializer;
            objectInitializer.world = world;
            objectInitializer.parent = this;
            auto result = std::dynamic_pointer_cast<SceneComponent>(attachedComponents.emplace_back(
                std::dynamic_pointer_cast<SceneComponent>(componentClass->GetConstructor()(objectInitializer, std::forward<Args>(args)...)))
            );
            AddChild(result);
            if (auto tickable = std::dynamic_pointer_cast<Tickable>(result))
            {
                tickableComponents.emplace_back(std::move(tickable));
            }
            return result;
        }

    protected:
        void ChildAdded(const std::shared_ptr<Object>& child) override;

        void ChildRemoved(const Object* object) override;
    
    private:
        void UpdateChildTransforms() const;
        
        Transform transform;

        std::vector<std::shared_ptr<SceneComponent>> attachedComponents;

        std::vector<std::shared_ptr<Tickable>> tickableComponents;

        World* world;

        IMPLEMENT_OBJECT_BASE(Actor)
    };
}
