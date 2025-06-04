#pragma once
#include <string>
#include <variant>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>

#include "core/datatypes/Transform.h"

#define PROPERTY_OFFSET(Class, Property) ((reinterpret_cast<char*>(&(Property)) - reinterpret_cast<char*>(this) ) - Vox::objectOffset<Object>(this))

#define CLASS_TYPE() std::remove_reference<decltype(*this)>::type

#define DECLARE_PROPERTY(Type, Name) Type Name;\
    Vox::Property _##Name {Vox::GetPropertyType<Type>(), PROPERTY_OFFSET(CLASS_TYPE(), Name)};

#define REGISTER_PROPERTY(Type, Name) propertiesInOut.emplace_back(#Name, Vox::GetPropertyType<Type>(), PROPERTY_OFFSET(CLASS_TYPE(), Name));

namespace Vox
{
    // start with an underscore because some of these are reserved keywords
    // this should be the **only** place names are close to these keywords
    enum class PropertyType : char
    {
        _bool,
        _int,
        _uint,
        _float,
        _string,
        _vec3,
        _quat,
        _transform,
        _invalid
    };

    using PropertyVariant = std::variant<bool, int, unsigned int, float, std::string, glm::vec3, glm::quat, Transform>;

    struct TypedPropertyVariant
    {
        PropertyType type = PropertyType::_invalid;
        PropertyVariant value;

        bool operator ==(const TypedPropertyVariant& other) const;

        [[nodiscard]] nlohmann::json Serialize() const;
    };

    static constexpr std::string GetPropertyTypeString(PropertyType type);

    static PropertyType GetPropertyTypeFromString(const std::string& propertyTypeString);

    template<typename Parent, typename Child>
    size_t objectOffset(Child* object)
    {
        return reinterpret_cast<char*>(static_cast<Parent*>(object)) - reinterpret_cast<char*>(object);    
    }
    
    template <typename T> consteval PropertyType GetPropertyType() { return PropertyType::_invalid; }
    template <> consteval PropertyType GetPropertyType<bool>() { return PropertyType::_bool; }
    template <> consteval PropertyType GetPropertyType<int>() { return PropertyType::_int; }
    template <> consteval PropertyType GetPropertyType<unsigned int>() { return PropertyType::_uint; }
    template <> consteval PropertyType GetPropertyType<float>() { return PropertyType::_float; }
    template <> consteval PropertyType GetPropertyType<std::string>() { return PropertyType::_string; }
    template <> consteval PropertyType GetPropertyType<glm::vec3>() { return PropertyType::_vec3; }
    template <> consteval PropertyType GetPropertyType<glm::quat>() { return PropertyType::_quat; }
    template <> consteval PropertyType GetPropertyType<Transform>() { return PropertyType::_transform; }
    
    struct Property
    {
        Property(const std::string& name, PropertyType propertyType, size_t offset);
        
        [[nodiscard]] PropertyType GetType() const;

        template <typename T>
        T* GetValuePtr(void* objectLocation) const
        {
            if (GetType() == GetPropertyType<T>())
            {
                return reinterpret_cast<T*>(static_cast<char*>(objectLocation) + propertyLocationOffset);
            }

            return nullptr;
        }

        template<typename T>
        T& GetValueChecked(void* objectLocation) const
        {
            assert(GetType() == GetPropertyType<T>());
            return *reinterpret_cast<T*>(static_cast<char*>(objectLocation) + propertyLocationOffset);
        }

        template<typename T>
        const T& GetValueChecked(const void* objectLocation) const
        {
            assert(GetType() == GetPropertyType<T>());
            return *reinterpret_cast<const T*>(static_cast<const char*>(objectLocation) + propertyLocationOffset);
        }

        /// Get the property name
        [[nodiscard]] const std::string& GetName() const;
        
        /// Get the property name, formatted for better readability
        /// Spaces and capitalization will be inserted
        [[nodiscard]] const std::string& GetFriendlyName() const;

        void SetValue(void* objectLocation, PropertyType type, const PropertyVariant& value) const;

        bool ValueEquals(const void* objectLocationA, const void* objectLocationB) const;

        static std::string FormatProperty(std::string propertyString);

        TypedPropertyVariant GetTypedVariant(const void* objectLocation) const;

        nlohmann::ordered_json Serialize(const void* objectLocation) const;

        static TypedPropertyVariant Deserialize(const nlohmann::ordered_json& property);
        
    private:
        size_t propertyLocationOffset = 0;
        PropertyType type = PropertyType::_invalid;
        std::string name;
        std::string friendlyName;
    };
}
