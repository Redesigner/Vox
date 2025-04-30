#include "Object.h"

namespace Vox
{
    Object::Object()
    {
    }

    void Object::InitProperties()
    {
        BuildProperties(properties);
    }

    std::vector<Property> Object::GetProperties() const
    {
        return properties;
    }
}
