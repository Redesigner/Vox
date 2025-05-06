#include "Actor.h"

namespace Vox
{
    void Actor::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        REGISTER_PROPERTY(Transform, transform)
    }

    const std::vector<std::unique_ptr<Component>>& Actor::GetComponents() const
    {
        return components;
    }

    void Actor::RegisterComponent(std::unique_ptr<Component> component)
    {
        components.emplace_back(std::move(component));
    }
}
