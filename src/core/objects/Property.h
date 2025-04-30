#pragma once
#include <string>
#include <variant>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace Vox
{
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

    template <typename T>
    consteval PropertyType GetPropertyType()
    {
        return PropertyType::_invalid;
    }

    template <>
    consteval PropertyType GetPropertyType<bool>()
    {
        return PropertyType::_bool;
    }

    template <>
    consteval PropertyType GetPropertyType<int>()
    {
        return PropertyType::_int;
    }
    
    struct Property
    {
        PropertyType GetType() const;

        template <typename T>
        T* GetValuePtr(void* objectLocation) const
        {
            if (GetType() == GetPropertyType<T>())
            {
                return static_cast<T*>(objectLocation + propertyLocationOffset);
            }

            return nullptr;
        }

        template<typename T>
        T& GetValueChecked(void* objectLocation) const
        {
            assert(GetType() == GetPropertyType<T>());
            return *static_cast<T*>(objectLocation + propertyLocationOffset);
        }
        
    private:
        size_t propertyLocationOffset;
        PropertyType type;
    };
}
