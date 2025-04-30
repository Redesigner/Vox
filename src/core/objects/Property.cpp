#include "Property.h"

namespace Vox
{
    Property::Property(std::string name, const PropertyType propertyType, const size_t offset)
        :propertyLocationOffset(offset), type(propertyType), name(std::move(name))
    {
    }

    const std::string& Property::GetName() const
    {
        return name;
    }
}
