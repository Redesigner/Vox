#include "Actor.h"

namespace Vox
{
    Actor::Actor(const ObjectInitializer& objectInitializer)
        :Object(objectInitializer), world(objectInitializer.world)
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
