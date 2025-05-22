#include "Actor.h"

namespace Vox
{
    Actor::Actor(const ObjectInitializer& objectInitializer)
        :world(objectInitializer.world)
    {
    }

    void Actor::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        REGISTER_PROPERTY(Transform, transform)
    }

    void Actor::PropertyChanged(const Property& property)
    {
        if (property.GetValuePtr<Transform>(this) == &transform)
        {
            UpdateChildTransforms();
        }
    }

    std::weak_ptr<Component> Actor::GetWeakChild(const SceneComponent* component) const
    {
        auto result = std::ranges::find_if(attachedComponents, [component](const std::shared_ptr<Component>& ownedComponent)
        {
            return ownedComponent.get() == component;
        });

        if (result != attachedComponents.end())
        {
            return *result;
        }

        return {};
    }

    std::weak_ptr<Component> Actor::GetWeakChild(const Component* component) const
    {
        auto result = std::ranges::find_if(components, [component](const std::shared_ptr<Component>& ownedComponent)
        {
            return ownedComponent.get() == component;
        });

        if (result != components.end())
        {
            return *result;
        }

        return {};
    }

    const std::vector<std::shared_ptr<Component>>& Actor::GetComponents() const
    {
        return components;
    }

    const std::vector<std::shared_ptr<SceneComponent>>& Actor::GetAttachedComponents() const
    {
        return attachedComponents;
    }

    void Actor::SetPosition(const glm::vec3 position)
    {
        if (transform.position != position)
        {
            transform.position = position;
            UpdateChildTransforms();
        }
    }

    void Actor::SetRotation(const glm::vec3 rotation)
    {
        if (transform.rotation != rotation)
        {
            transform.rotation = rotation;
            UpdateChildTransforms();
        }
    }

    void Actor::SetScale(glm::vec3 scale)
    {
        if (transform.scale != scale)
        {
            transform.scale = scale;
            UpdateChildTransforms();
        }
    }

    void Actor::SetTransform(const Transform& transformIn)
    {
        transform = transformIn;
        UpdateChildTransforms();
    }

    void Actor::Tick(float deltaTime)
    {
        for (auto& tickableComponent : tickableComponents)
        {
            tickableComponent->Tick(deltaTime);
        }
    }

    World* Actor::GetWorld() const
    {
        return world;
    }

#ifdef EDITOR
    void Actor::Select() const
    {
        for (const std::shared_ptr<SceneComponent>& component : attachedComponents)
        {
            component->Select();
        }
    }
#endif

    const Transform& Actor::GetTransform() const
    {
        return transform;
    }

    void Actor::UpdateChildTransforms() const
    {
        for (auto& attachedComponent : attachedComponents)
        {
            attachedComponent->UpdateParentTransform(transform);
        }
    }
}
