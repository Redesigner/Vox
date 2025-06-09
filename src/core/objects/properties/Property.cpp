#include "Property.h"

#include <nlohmann/json.hpp>
#include <regex>
#include <glm/detail/type_quat.hpp>

namespace Vox
{
    nlohmann::json TypedPropertyVariant::Serialize() const
    {
        using Json = nlohmann::json;

        Json result;
        result["type"] = GetPropertyTypeString(type);
        switch (type)
        {
        case PropertyType::_bool:
            result["value"] = std::get<bool>(value);
            break;

        case PropertyType::_int:
            result["value"] = std::get<int>(value);
            break;

        case PropertyType::_uint:
            result["value"] = std::get<unsigned int>(value);
            break;

        case PropertyType::_float:
            result["value"] = std::get<float>(value);
            break;

        case PropertyType::_string:
            result["value"] = std::get<std::string>(value);
            break;

        case PropertyType::_vec3:
            const glm::vec3 vec3Value = std::get<glm::vec3>(value);
            result["value"] = {vec3Value.x, vec3Value.y, vec3Value.z};
            break;

        case PropertyType::_quat:
            const glm::quat quatValue = std::get<glm::quat>(value);
            result["value"] = {quatValue.x, quatValue.y, quatValue.z, quatValue.w};
            break;

        case PropertyType::_transform:
        {
            const Transform transform = std::get<Transform>(value);
            result["value"] = {
                transform.position.x, transform.position.y, transform.position.z,
                transform.rotation.x, transform.rotation.y, transform.rotation.z,
                transform.scale.x, transform.scale.y, transform.scale.z
            };
            break;
        }

        case PropertyType::_invalid:
            break;
        }

        return result;
    }

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
        case PropertyType::_assetPtr:
            return "assetPtr";
        case PropertyType::_invalid:
            return "invalid";
        }

        return "invalid";
    }

    bool TypedPropertyVariant::operator==(const TypedPropertyVariant& other) const
    {
        return type == other.type && value == other.value;
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

        case PropertyType::_assetPtr:
            GetValueChecked<AssetPtr>(objectLocation) = std::get<AssetPtr>(value);
            break;

        case PropertyType::_invalid:
            break;
        }
    }

    bool Property::ValueEquals(const void* objectLocationA, const void* objectLocationB) const
    {
        if (objectLocationA == nullptr || objectLocationB == nullptr)
        {
            return false;
        }

        return GetTypedVariant(objectLocationA) == GetTypedVariant(objectLocationB);
    }

    std::string Property::FormatProperty(std::string propertyString)
    {
        assert(!propertyString.empty());
        propertyString = std::regex_replace(propertyString, std::regex("([a-z](?=[A-Z0-9]))"), "$1 ");
        propertyString[0] = static_cast<char>(toupper(propertyString[0]));
        return propertyString;
    }

    TypedPropertyVariant Property::GetTypedVariant(const void* objectLocation) const
    {
        switch (type)
        {
        case PropertyType::_bool:
            return {type, GetValueChecked<bool>(objectLocation)};

        case PropertyType::_int:
            return {type, GetValueChecked<int>(objectLocation)};

        case PropertyType::_uint:
            return {type, GetValueChecked<unsigned int>(objectLocation)};

        case PropertyType::_float:
            return {type, GetValueChecked<float>(objectLocation)};

        case PropertyType::_string:
            return {type, GetValueChecked<std::string>(objectLocation)};

        case PropertyType::_vec3:
            return {type, GetValueChecked<glm::vec3>(objectLocation)};

        case PropertyType::_quat:
            return {type, GetValueChecked<glm::quat>(objectLocation)};

        case PropertyType::_transform:
            return {type, GetValueChecked<Transform>(objectLocation)};

        case PropertyType::_assetPtr:
            return {type, GetValueChecked<AssetPtr>(objectLocation)};

        case PropertyType::_invalid:
            return {type, nullptr};
        }

        return {PropertyType::_invalid, nullptr};
    }

    nlohmann::ordered_json Property::Serialize(const void* objectLocation) const
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
            const auto& transform = GetValueChecked<Transform>(objectLocation);
            propertyJson[name]["value"] = {
                transform.position.x, transform.position.y, transform.position.z,
                transform.rotation.x, transform.rotation.y, transform.rotation.z,
                transform.scale.x, transform.scale.y, transform.scale.z
            };
            break;
        }

        case PropertyType::_assetPtr:
        {
            const auto& assetPtr = GetValueChecked<AssetPtr>(objectLocation);
            const json assetPtrJson = assetPtr.Serialize();
            propertyJson[name] = assetPtrJson;
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

    TypedPropertyVariant Property::Deserialize(const nlohmann::ordered_json& property)
    {
        using Json = nlohmann::json;
        if (!property.contains("type") || !property["type"].is_string() || !property.contains("value"))
        {
            return {PropertyType::_invalid, {}};
        }

        PropertyType type = GetPropertyTypeFromString(property["type"]);
        const Json& propertyValue = property["value"];

        PropertyVariant variant;
        switch(type)
        {
        case PropertyType::_bool:
            return propertyValue.is_boolean() ? TypedPropertyVariant{type, propertyValue.get<Json::value_type::boolean_t>()} :
            TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_int:
            return propertyValue.is_number_integer() ? TypedPropertyVariant{type, static_cast<int>(propertyValue.get<Json::value_type::number_integer_t>())} :
            TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_uint:
            return propertyValue.is_number_integer() ? TypedPropertyVariant{type, static_cast<unsigned int>(propertyValue.get<Json::value_type::number_integer_t>())} :
            TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_float:
            return propertyValue.is_number_float() ? TypedPropertyVariant{type, static_cast<float>(propertyValue.get<Json::value_type::number_float_t>())} :
            TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_string:
            return propertyValue.is_string() ? TypedPropertyVariant{type, propertyValue.get<Json::value_type::string_t>()} :
            TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_vec3:
            if (propertyValue.is_array())
            {
                if (propertyValue.size() == 3)
                {
                    if (propertyValue[0].is_number_float() && propertyValue[1].is_number_float() && propertyValue[2].is_number_float())
                    {
                        return TypedPropertyVariant{type, glm::vec3(propertyValue[0], propertyValue[1], propertyValue[2])};
                    }
                }
            }
            return TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_quat:
            if (propertyValue.is_array())
            {
                if (propertyValue.size() == 3)
                {
                    if (propertyValue[0].is_number_float() && propertyValue[1].is_number_float() && propertyValue[2].is_number_float() && propertyValue[3].is_number_float())
                    {
                        return TypedPropertyVariant{type, glm::quat(propertyValue[0], propertyValue[1], propertyValue[2], propertyValue[3])};
                    }
                }
            }
            return TypedPropertyVariant{PropertyType::_invalid, {}};

        case PropertyType::_transform:
        {
            if (propertyValue.is_array())
            {
                if (propertyValue.size() == 9)
                {
                    for (int i = 0; i < 9; ++i)
                    {
                        if (!propertyValue[i].is_number_float())
                        {
                            return TypedPropertyVariant{PropertyType::_invalid, {}};
                        }
                    }
                    return TypedPropertyVariant{type, Transform(
                        {propertyValue[0], propertyValue[1], propertyValue[2]},
                        {propertyValue[3], propertyValue[4], propertyValue[5]},
                        {propertyValue[6], propertyValue[7], propertyValue[8]}
                    )};
                }
            }
            return TypedPropertyVariant{PropertyType::_invalid, {}};
        }

        case PropertyType::_assetPtr:
        {
            std::optional<AssetPtr> assetPtr = AssetPtr::Deserialize(propertyValue);
            if (assetPtr.has_value())
            {
                return TypedPropertyVariant{PropertyType::_assetPtr, assetPtr.value()};
            }
            return TypedPropertyVariant{PropertyType::_invalid, {}};
        }

        case PropertyType::_invalid:
            return TypedPropertyVariant{PropertyType::_invalid, {}};
        }

        return TypedPropertyVariant{PropertyType::_invalid, {}};
    }
}
