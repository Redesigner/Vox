#pragma once
#include <functional>

#include "Object.h"
#include "Property.h"

namespace Vox
{
    struct ObjectInitializer;
    class Object;

    class ObjectClass
    {
    public:
        using Constructor = std::function<std::shared_ptr<Object>(const ObjectInitializer&)>;
        ObjectClass(Constructor constructor, const ObjectClass* parent, std::vector<Property> properties);
        virtual ~ObjectClass() = default;

        [[nodiscard]] Constructor GetConstructor() const;
        [[nodiscard]] const ObjectClass* GetParentClass() const;
        [[nodiscard]] const std::vector<Property>& GetProperties() const;
        [[nodiscard]] Property* GetPropertyByType(PropertyType type) const;
        [[nodiscard]] Property* GetPropertyByName(const std::string& name) const;

        template <typename T>
        [[nodiscard]] bool IsA() const requires Derived<T, Object>
        {
            const ObjectClass* matchingClass = T::Class();
            for (const ObjectClass* objectClass = this; objectClass != nullptr; objectClass = objectClass->GetParentClass())
            {
                if (objectClass == matchingClass)
                {
                    return true;
                }
            }
            return false;
        }

        template <>
        [[nodiscard]] bool IsA<Object>() const;

#ifdef EDITOR
        [[nodiscard]] bool CanBeRenamed() const;
#endif

    protected:
        Constructor constructor;
        const ObjectClass* parentClass;
        std::vector<Property> properties;
        bool canBeRenamed = false;
    };
}
