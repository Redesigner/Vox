#include "Property.h"

#include <nlohmann/json.hpp>
#include <regex>
#include <glm/detail/type_quat.hpp>

namespace Vox
{
    constexpr std::string GetPropertyTypeString(const PropertyType type)
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

    PropertyType GetPropertyTypeFromString(const std::string& propertyTypeString)
    {
        if (propertyTypeString == GetPropertyTypeString(PropertyType::_bool))
        {
            return PropertyType::_bool;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_int))
        {
            return PropertyType::_int;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_uint))
        {
            return PropertyType::_uint;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_float))
        {
            return PropertyType::_float;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_string))
        {
            return PropertyType::_string;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_vec3))
        {
            return PropertyType::_vec3;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_quat))
        {
            return PropertyType::_quat;
        }

        if (propertyTypeString == GetPropertyTypeString(PropertyType::_transform))
        {
            return PropertyType::_transform;
        }

        return PropertyType::_invalid;
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

    void Property::SetValue(void* objectLocation, PropertyType type, const PropertyVariant& value) const
    {
        switch (type)
        {
        case PropertyType::_bool:
            GetValueChecked<bool>(objectLocation) = std::get<bool>(value);
            break;

        case PropertyType::_int:
            GetValueChecked<int>(objectLocation) = std::get<int>(value);
            break;

        case PropertyType::_uint:
            GetValueChecked<unsigned int>(objectLocation) = std::get<unsigned int>(value);
            break;

        case PropertyType::_float:
            GetValueChecked<float>(objectLocation) = std::get<float>(value);
            break;

        case PropertyType::_string:
            GetValueChecked<std::string>(objectLocation) = std::get<std::string>(value);
            break;

        case PropertyType::_vec3:
            GetValueChecked<glm::vec3>(objectLocation) = std::get<glm::vec3>(value);
            break;

        case PropertyType::_quat:
            GetValueChecked<glm::quat>(objectLocation) = std::get<glm::quat>(value);
            break;

        case PropertyType::_transform:
            GetValueChecked<Transform>(objectLocation) = std::get<Transform>(value);
            break;

        case PropertyType::_invalid:
            break;
        }
    }

    bool Property::ValueEquals(void* objectLocationA, void* objectLocationB) const
    {
        switch (type)
        {
        case PropertyType::_bool:
            return GetValueChecked<bool>(objectLocationA) == GetValueChecked<bool>(objectLocationB);

        case PropertyType::_int:
            return GetValueChecked<int>(objectLocationA) == GetValueChecked<int>(objectLocationB);

        case PropertyType::_uint:
            return GetValueChecked<unsigned int>(objectLocationA) == GetValueChecked<unsigned int>(objectLocationB);

        case PropertyType::_float:
            return GetValueChecked<float>(objectLocationA) == GetValueChecked<float>(objectLocationB);

        case PropertyType::_string:
            return GetValueChecked<std::string>(objectLocationA) == GetValueChecked<std::string>(objectLocationB);

        case PropertyType::_vec3:
            return GetValueChecked<glm::vec3>(objectLocationA) == GetValueChecked<glm::vec3>(objectLocationB);

        case PropertyType::_quat:
            return GetValueChecked<glm::quat>(objectLocationA) == GetValueChecked<glm::quat>(objectLocationB);

        case PropertyType::_transform:
            return GetValueChecked<Transform>(objectLocationA) == GetValueChecked<Transform>(objectLocationB);

        default:
        case PropertyType::_invalid:
            return false;
        }
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
                    transform.rotation.x, transform.rotation.y, transform.rotation.z,
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

    std::pair<PropertyType, PropertyVariant> Property::Deserialize(
        const nlohmann::ordered_json& property)
    {
        using Json = nlohmann::json;
        using TypedVariant = std::pair<PropertyType, PropertyVariant>;
        if (!property.contains("type") || !property["type"].is_string() || !property.contains("value"))
        {
            return TypedVariant{PropertyType::_invalid, {}};
        }

        PropertyType type = GetPropertyTypeFromString(property["type"]);
        const Json& propertyValue = property["value"];

        PropertyVariant variant;
        switch(type)
        {
        case PropertyType::_bool:
            return propertyValue.is_boolean() ? TypedVariant{type, propertyValue.get<Json::value_type::boolean_t>()} :
            TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_int:
            return propertyValue.is_number_integer() ? TypedVariant{type, static_cast<int>(propertyValue.get<Json::value_type::number_integer_t>())} :
            TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_uint:
            return propertyValue.is_number_integer() ? TypedVariant{type, static_cast<unsigned int>(propertyValue.get<Json::value_type::number_integer_t>())} :
            TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_float:
            return propertyValue.is_number_float() ? TypedVariant{type, static_cast<float>(propertyValue.get<Json::value_type::number_float_t>())} :
            TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_string:
            return propertyValue.is_string() ? TypedVariant{type, propertyValue.get<Json::value_type::string_t>()} :
            TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_vec3:
            if (propertyValue.is_array())
            {
                if (propertyValue.size() == 3)
                {
                    if (propertyValue[0].is_number_float() && propertyValue[1].is_number_float() && propertyValue[2].is_number_float())
                    {
                        return TypedVariant{type, glm::vec3(propertyValue[0], propertyValue[1], propertyValue[2])};
                    }
                }
            }
            return TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_quat:
            if (propertyValue.is_array())
            {
                if (propertyValue.size() == 3)
                {
                    if (propertyValue[0].is_number_float() && propertyValue[1].is_number_float() && propertyValue[2].is_number_float() && propertyValue[3].is_number_float())
                    {
                        return TypedVariant{type, glm::quat(propertyValue[0], propertyValue[1], propertyValue[2], propertyValue[3])};
                    }
                }
            }
            return TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_transform:
            if (propertyValue.is_array())
            {
                if (propertyValue.size() == 9)
                {
                    for (int i = 0; i < 9; ++i)
                    {
                        if (!propertyValue[i].is_number_float())
                        {
                            return TypedVariant{PropertyType::_invalid, {}};
                        }
                    }
                    return TypedVariant{type, Transform(
                        {propertyValue[0], propertyValue[1], propertyValue[2]},
                        {propertyValue[3], propertyValue[4], propertyValue[5]},
                        {propertyValue[6], propertyValue[7], propertyValue[8]}
                    )};
                }
            }
            return TypedVariant{PropertyType::_invalid, {}};

        case PropertyType::_invalid:
        default:
            return TypedVariant{PropertyType::_invalid, {}};
        }
    }
}
