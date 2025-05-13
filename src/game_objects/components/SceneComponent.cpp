#include "SceneComponent.h"

#include "core/objects/actor/Actor.h"

namespace Vox
{
    void SceneComponent::PropertyChanged(const Property& property)
    {
        if (property.GetValuePtr<Transform>(this) == &localTransform)
        {
            UpdateTransform();
        }
    }

    void SceneComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        REGISTER_PROPERTY(Transform, localTransform)
    }

    void SceneComponent::UpdateParentTransform(const Transform& transform)
    {
        if (worldTransform != transform)
        {
            worldTransform = Transform(transform.GetMatrix() * localTransform.GetMatrix());
            OnTransformUpdated();
        }
    }

    void SceneComponent::SetPosition(const glm::vec3 position)
    {
        if (localTransform.position != position)
        {
            localTransform.position = position;
            UpdateTransform();
        }
    }

    void SceneComponent::SetRotation(const glm::vec3 rotation)
    {
        if (localTransform.rotation != rotation)
        {
            localTransform.rotation = rotation;
            UpdateTransform();
        }
    }

    void SceneComponent::SetScale(const glm::vec3 scale)
    {
        if (localTransform.scale != scale)
        {
            localTransform.scale = scale;
            UpdateTransform();
        }
    }

    void SceneComponent::SetTransform(const Transform& transformIn)
    {
        localTransform = transformIn;
        UpdateTransform();
    }

    glm::vec3 SceneComponent::GetLocalPosition() const
    {
        return localTransform.position;
    }

    glm::quat SceneComponent::GetLocalRotation() const
    {
        return localTransform.rotation;
    }

    glm::vec3 SceneComponent::GetLocalScale() const
    {
        return localTransform.scale;
    }

    Transform SceneComponent::GetLocalTransform() const
    {
        return localTransform;
    }

    Transform SceneComponent::GetWorldTransform() const
    {
        return worldTransform;
    }

    void SceneComponent::UpdateTransform()
    {
        if (const Actor* parent = GetParent())
        {
            worldTransform = Transform(parent->GetTransform().GetMatrix() * localTransform.GetMatrix());
        }
        else
        {
            worldTransform = localTransform;
        }

        OnTransformUpdated();
    }
}
