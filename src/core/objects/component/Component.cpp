#include "Component.h"

#include "core/objects/actor/Actor.h"

namespace Vox
{
    Component::Component(const ObjectInitializer& objectInitializer)
        :Object(objectInitializer)
    {
    }

    Actor* Component::GetActor() const
    {
        if (auto* parent = dynamic_cast<Actor*>(GetParent()))
        {
            return parent;
        }

        return nullptr;
    }

    void Component::BuildProperties(std::vector<Property>& propertiesInOut)
    {
    }
}
