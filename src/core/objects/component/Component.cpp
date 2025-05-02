#include "Component.h"

namespace Vox
{
    Actor* Component::GetParent() const
    {
        return parent;
    }

    void Component::BuildProperties(std::vector<Property>& propertiesInOut)
    {
    }
}
