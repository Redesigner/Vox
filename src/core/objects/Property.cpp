#include "Property.h"

#include <nlohmann/json.hpp>
#include <regex>
#include <glm/detail/type_quat.hpp>

namespace Vox
{
    std::string GetPropertyTypeString(const PropertyType type)
    {
        switch (type)
        {
        case PropertyType::_bool:
            return "bool";
        case PropertyType::_int:
            return "int";
        case PropertyType::_uint:
            return "uint";
        case PropertyType::_float:
            return "float";
        case PropertyType::_string:
            return "string";
        case PropertyType::_vec3:
            return "vec3";
        case PropertyType::_quat:
            return "quat";
        case PropertyType::_transform:
            return "transform";
        case PropertyType::_invalid:
        default:
            return "invalid";
        }
    }

    Property::Property(const std::string& name, const PropertyType propertyType, const size_t offset)
        :propertyLocationOffset(offset), type(propertyType), name(name), friendlyName(FormatProperty(name))
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

    const std::string& Property::GetFriendlyName() const
    {
        return friendlyName;
    }

    std::string Property::FormatProperty(std::string propertyString)
    {
        assert(!propertyString.empty());
        propertyString = std::regex_replace(propertyString, std::regex("([a-z](?=[A-Z0-9]))"), "$1 ");
        propertyString[0] = static_cast<char>(toupper(propertyString[0]));
        return propertyString;
    }

    nlohmann::ordered_json Property::Serialize(void* objectLocation) const
    {
        using json = nlohmann::ordered_json;

        json propertyJson {};
        propertyJson[name]["type"] = GetPropertyTypeString(type);

        switch (type)
        {
        case PropertyType::_bool:
            {
                propertyJson[name]["value"] = GetValueChecked<bool>(objectLocation);
                break;
            }
        case PropertyType::_int:
            {
                propertyJson[name]["value"] = GetValueChecked<int>(objectLocation);
                break;
            }
        case PropertyType::_uint:
            {
                propertyJson[name]["value"] = GetValueChecked<unsigned int>(objectLocation);
                break;
            }
        case PropertyType::_float:
            {
                propertyJson[name]["value"] = GetValueChecked<float>(objectLocation);
                break;
            }
        case PropertyType::_string:
            {
                propertyJson[name]["value"] = GetValueChecked<std::string>(objectLocation);
                break;
            }
        case PropertyType::_vec3:
            {
                const glm::vec3 vector = GetValueChecked<glm::vec3>(objectLocation);
                propertyJson[name]["value"] = {vector.x, vector.y, vector.z};
                break;
            }
        case PropertyType::_quat:
            {
                const glm::quat quaternion = GetValueChecked<glm::quat>(objectLocation);
                propertyJson[name]["value"] = {quaternion.x, quaternion.y, quaternion.z, quaternion.w};
                break;
            }
        case PropertyType::_transform:
            {
                const Transform& transform = GetValueChecked<Transform>(objectLocation);
                propertyJson[name]["value"] = {
                    transform.position.x, transform.position.y, transform.position.z,
                    transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w,
                    transform.scale.x, transform.scale.y, transform.scale.z
                };
                break;
            }
        case PropertyType::_invalid:
            {
                propertyJson[name]["value"] = 0;
                break;
            }
        }
        return propertyJson;
    }
}
