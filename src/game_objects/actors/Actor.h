#pragma once
#include "../../core/objects/interfaces/Tickable.h"
#include "core/objects/Object.h"
#include "core/objects/ObjectClass.h"
#include "../components/Component.h"
#include "../components/scene_component/SceneComponent.h"


namespace  Vox
{
    class Component;
    class World;

    class Actor : public Object
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

        [[nodiscard]] World* GetWorld() const;

        [[nodiscard]] std::shared_ptr<SceneComponent> GetRootComponent() const;

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
            auto newComponent = Component::Create<T>(objectInitializer, std::forward<Args>(args)...);
            if (rootComponent)
            {
                rootComponent->AttachComponent(newComponent);
            }
            else
            {
                rootComponent = newComponent;
            }
            AddChild(newComponent);
            newComponent->PostConstruct();
            return newComponent;
        }

        template <typename... Args>
        std::shared_ptr<SceneComponent> AttachComponent(const ObjectClass* componentClass, Args&&... args)
        {
            assert(componentClass->IsA<SceneComponent>());

            ObjectInitializer objectInitializer;
            objectInitializer.world = world;
            objectInitializer.parent = this;
            auto newComponent = componentClass->Construct<SceneComponent>(objectInitializer);
            AddChild(newComponent);
            if (rootComponent)
            {
                rootComponent->AttachComponent(newComponent);
            }
            else
            {
                rootComponent = newComponent;
            }
            return newComponent;
        }

        /**
         * @brief Get all children
         * @return List of all children
         */
        [[nodiscard]] const std::vector<std::shared_ptr<Component>>& GetChildren() const;

        /**
         * @brief Get a shared_ptr of a child component
         * @param component Component to get the shared_ptr of
         * @return shared_ptr. Can be nullptr if object is not a child of this
         */
        [[nodiscard]] std::shared_ptr<Component> GetSharedChild(const Component* component) const;

        /**
         * @brief Lookup a child component by name
         * @param name Name to search for
         * @return shared_ptr. Can be nullptr if no child has this name
         */
        [[nodiscard]] std::shared_ptr<Component> GetChildByName(const std::string& name) const;

        /**
         * @brief Add an already constructed component to this one, as a child. Implies ownership,
         * even though other objects may have weak_ptrs to the child. The child will be renamed
         * if its name matches another child.
         * @param child Child to be added
         */
        void AddChild(const std::shared_ptr<Component>& child);

        /**
         * @brief Remove a child object
         * @param component component to compare raw_ptrs with
         * @return true if the object was removed, otherwise false
         */
        bool RemoveChild(const Component* component);

    protected:
        /**
         * @brief Construct and add a child to this.
         * @tparam T Object type to construct. Must be derived from Object
         * @param name Child object's name
         */
        template<typename T>
        void CreateChild(const std::string& name) requires Derived<T, Object>
        {
            std::shared_ptr<T> newObject = T::template GetConstructor<T>()(ObjectInitializer(this));
            newObject->SetName(name);
            AddChild(newObject);
        }

        /**
         * @brief Method called when a child is removed. Can be used for additional
         * cleanup logic, for instance removing an attachment from an actor
         * @param child ptr to the component that was removed. May be null
         */
        void ChildRemoved(const Component* child);

        /**
         * @brief Method called when a child is added to this.
         * @param child shared_ptr to the object that was added
         */
        void ChildAdded(const std::shared_ptr<Component>& child);
    
    private:
        void UpdateChildTransforms() const;

        Transform transform;

        std::shared_ptr<SceneComponent> rootComponent;

        std::vector<std::shared_ptr<Component>> children;

        World* world;

        IMPLEMENT_OBJECT_BASE(Actor)
    };
}
