#pragma once
#include <string>
#include <variant>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#define PROPERTY_OFFSET(Class, Property) ((reinterpret_cast<char*>(&(Property)) - reinterpret_cast<char*>(this) ) - Vox::objectOffset<Object>(this))

#define CLASS_TYPE() std::remove_reference<decltype(*this)>::type

#define DECLARE_PROPERTY(Type, Name) Type Name;\
    Vox::Property _##Name {Vox::GetPropertyType<Type>(), PROPERTY_OFFSET(CLASS_TYPE(), Name)};

#define REGISTER_PROPERTY(Type, Name) propertiesInOut.emplace_back(Property::FormatProperty(#Name), Vox::GetPropertyType<Type>(), PROPERTY_OFFSET(CLASS_TYPE(), Name));

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
        _invalid
    };

    typedef unsigned int uint;

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
    
    struct Property
    {
        Property(std::string name, PropertyType propertyType, size_t offset);
        
        PropertyType GetType() const;

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

        const std::string& GetName() const;

        static std::string FormatProperty(std::string propertyString);
        
    private:
        size_t propertyLocationOffset = 0;
        PropertyType type = PropertyType::_invalid;
        std::string name;
    };
}
