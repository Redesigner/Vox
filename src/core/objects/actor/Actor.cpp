#include "Actor.h"

#include "core/math/Strings.h"
#include "core/objects/world/World.h"

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
        return rootComponent->GetAttachments();
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

    World* Actor::GetWorld() const
    {
        return world;
    }

    std::shared_ptr<SceneComponent> Actor::GetRootComponent() const
    {
        return rootComponent;
    }

#ifdef EDITOR
    void Actor::Select() const
    {
        // for (const std::shared_ptr<SceneComponent>& component : attachedComponents)
        // {
        //     component->Select();
        // }
    }
#endif

    const Transform& Actor::GetTransform() const
    {
        return transform;
    }

    const std::vector<std::shared_ptr<Component>>& Actor::GetChildren() const
    {
        return children;
    }

    std::shared_ptr<Component> Actor::GetSharedChild(const Component* component) const
    {
        const auto result = std::ranges::find_if(children, [component](const std::shared_ptr<Component>& childObject)
        {
            return childObject.get() == component;
        });

        if (result != children.end())
        {
            return *result;
        }

        return {};
    }

    std::shared_ptr<Component> Actor::GetChildByName(const std::string& name) const
    {
        for (const auto& child : children)
        {
            if (child->GetDisplayName() == name)
            {
                return child;
            }
        }

        return {};
    }

    void Actor::AddChild(const std::shared_ptr<Component>& child)
    {
        for (const auto& existingChild : children)
        {
            // check name collisions
            if (existingChild->GetDisplayName() == child->GetDisplayName())
            {
                child->SetName(IncrementString(child->GetDisplayName()));
            }
        }
        if (world)
        {
            if (std::shared_ptr<Tickable> tickable = std::dynamic_pointer_cast<Tickable>(child))
            {
                world->GetTickManager().RegisterTickable(tickable);
            }
        }
        ChildAdded(children.emplace_back(child));
    }

    bool Actor::RemoveChild(const Component* component)
    {
        const bool childErased = std::erase_if(children, [component](const std::shared_ptr<Component>& child)
        {
            return child.get() == component;
        });

        if (childErased)
        {
            ChildRemoved(component);
        }
        return childErased;
    }

    void Actor::ChildAdded(const std::shared_ptr<Component>& child)
    {
        // Only check non-native children if they should be attached
        if (child->native)
        {
            return;
        }

        // if (const std::shared_ptr<SceneComponent> sceneComponent = std::dynamic_pointer_cast<SceneComponent>(child))
        // {
        //     attachedComponents.emplace_back(sceneComponent);
        // }
    }

    void Actor::ChildRemoved(const Component* child)
    {
        // std::erase_if(attachedComponents, [child](const std::shared_ptr<SceneComponent>& component)
        // {
        //     return component.get() == child;
        // });
    }

    void Actor::UpdateChildTransforms() const
    {
        /*for (auto& attachedComponent : attachedComponents)
        {
            attachedComponent->UpdateParentTransform(transform);
        }*/
    }
}
