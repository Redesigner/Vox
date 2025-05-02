#include "Property.h"

#include <regex>

namespace Vox
{
    Property::Property(std::string name, const PropertyType propertyType, const size_t offset)
        :propertyLocationOffset(offset), type(propertyType), name(std::move(name))
    {
    }

    PropertyType Property::GetType() const
    {
        return type;
    }

    const std::string& Property::GetName() const
    {
        return name;
    }

    std::string Property::FormatProperty(std::string propertyString)
    {
        assert(!propertyString.empty());
        propertyString = std::regex_replace(propertyString, std::regex("([a-z](?=[A-Z0-9]))"), "$1 ");
        propertyString[0] = toupper(propertyString[0]);
        return propertyString;
    }
}
