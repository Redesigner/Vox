#include "SceneComponent.h"

#include "core/objects/actor/Actor.h"

namespace Vox
{
    SceneComponent::SceneComponent(const ObjectInitializer& objectInitializer)
        :Component(objectInitializer), parentAttachment(nullptr)
    {
    }

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

    void SceneComponent::AttachComponent(const std::shared_ptr<SceneComponent>& attachment)
    {
        attachedComponents.emplace_back(attachment);
        attachment->SetParent(GetParent());
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

    World* SceneComponent::GetWorld() const
    {
        return GetParent()->GetWorld();
    }

    const Actor* SceneComponent::GetRoot() const
    {
        return nullptr;
    }

    std::vector<std::string> SceneComponent::GetRootPath() const
    {
        std::vector resultPath = { GetDisplayName() };
        for (const SceneComponent* attachment = GetParentAttachment(); attachment; attachment = attachment->GetParentAttachment())
        {
            if (parentAttachment->GetParent())
            {
                resultPath.emplace_back(parentAttachment->GetDisplayName());
            }
        }
        return resultPath;
    }

    std::shared_ptr<SceneComponent> SceneComponent::GetChildByPath(const std::vector<std::string>& path) const
    {
        if (path.empty())
        {
            return std::dynamic_pointer_cast<SceneComponent>(GetSharedThis());
        }

        std::shared_ptr<SceneComponent> currentObject;
        for (auto iter = path.rbegin(); iter != path.rend(); ++iter)
        {
            currentObject = currentObject ? currentObject->GetAttachmentByName(*iter) : GetAttachmentByName(*iter);
            if (!currentObject)
            {
                return nullptr;
            }
        }
        return currentObject;
    }

    std::shared_ptr<SceneComponent> SceneComponent::GetAttachmentByName(const std::string& name) const
    {
        const auto& result = std::ranges::find_if(attachedComponents, [name](const std::shared_ptr<SceneComponent>& component)
        {
            return component->GetDisplayName() == name;
        });

        if (result == attachedComponents.end())
        {
            return {};
        }

        return *result;
    }

    SceneComponent* SceneComponent::GetParentAttachment() const
    {
        return parentAttachment;
    }

    const std::vector<std::shared_ptr<SceneComponent>>& SceneComponent::GetAttachments() const
    {
        return attachedComponents;
    }

    void SceneComponent::UpdateTransform()
    {
        if (const SceneComponent* attachment = GetParentAttachment())
        {
            worldTransform = Transform(attachment->GetWorldTransform().GetMatrix() * localTransform.GetMatrix());
        }
        else
        {
            worldTransform = localTransform;
        }

        OnTransformUpdated();

        for (const auto& attachment: attachedComponents)
        {
            attachment->UpdateParentTransform(worldTransform);
        }
    }
}
