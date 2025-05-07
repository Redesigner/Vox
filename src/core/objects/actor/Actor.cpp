#include "Actor.h"

namespace Vox
{
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

    const std::vector<std::unique_ptr<Component>>& Actor::GetComponents() const
    {
        return components;
    }

    const std::vector<std::unique_ptr<SceneComponent>>& Actor::GetAttachedComponents() const
    {
        return attachedComponents;
    }

    void Actor::SetPosition(const glm::vec3 position)
    {
        transform.position = position;
        UpdateChildTransforms();
    }

    void Actor::SetRotation(const glm::vec3 rotation)
    {
        transform.rotation = rotation;
        UpdateChildTransforms();
    }

    void Actor::SetScale(glm::vec3 scale)
    {
        transform.scale = scale;
        UpdateChildTransforms();
    }

    void Actor::SetTransform(const Transform& transformIn)
    {
        transform = transformIn;
        UpdateChildTransforms();
    }

    const Transform& Actor::GetTransform() const
    {
        return transform;
    }

    void Actor::RegisterComponent(std::unique_ptr<Component> component)
    {
        components.emplace_back(std::move(component));
    }

    void Actor::AttachComponent(std::unique_ptr<SceneComponent> component)
    {
        attachedComponents.emplace_back(std::move(component));
    }

    void Actor::UpdateChildTransforms() const
    {
        for (auto& attachedComponent : attachedComponents)
        {
            attachedComponent->UpdateParentTransform(transform);
        }
    }
}
