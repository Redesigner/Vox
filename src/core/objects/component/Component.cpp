#include "Component.h"

#include "core/logging/Logging.h"
#include "core/objects/actor/Actor.h"

namespace Vox
{
    Component::Component(const ObjectInitializer& objectInitializer)
        :Object(objectInitializer), parent(objectInitializer.parent)
    {
    }

    void Component::BuildProperties(std::vector<Property>& propertiesInOut)
    {
    }

    Actor* Component::GetParent() const
    {
        return parent;
    }

    void Component::SetParent(Actor* parentIn) const
    {
        assert(parent);

        const std::shared_ptr<Component> object = GetSharedThis();
        // ReSharper disable once CppDFANullDereference -- caught by assert
        parent->RemoveChild(this);
        parentIn->AddChild(object);
    }

    std::shared_ptr<Component> Component::GetSharedThis() const
    {
        if (!parent)
        {
            VoxLog(Display, Game, "Failed to get shared this. Parent was nullptr.");
            return {};
        }

        if (auto result = parent->GetSharedChild(this))
        {
            return result;
        }

        assert(false && "Child object may be orphaned");
        return {};
    }
}
