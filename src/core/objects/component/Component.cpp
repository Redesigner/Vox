#include "Component.h"

#include "core/objects/actor/Actor.h"

namespace Vox
{
    Component::Component(Actor* parent)
        :parent(parent)
    {
    }

    Actor* Component::GetParent() const
    {
        return parent;
    }

    void Component::BuildProperties(std::vector<Property>& propertiesInOut)
    {
    }

    std::weak_ptr<Component> Component::GetWeakThis() const
    {
        if (!parent)
        {
            return {};
        }
        return parent->GetWeakChild(this);
    }
}
