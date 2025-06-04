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
        ObjectClass(std::string name, Constructor constructor, const std::shared_ptr<ObjectClass>& parent, std::vector<Property> properties);
        virtual ~ObjectClass() = default;

        [[nodiscard]] Constructor GetConstructor() const;
        [[nodiscard]] std::shared_ptr<ObjectClass> GetParentClass() const;
        [[nodiscard]] const std::vector<Property>& GetProperties() const;
        [[nodiscard]] Property* GetPropertyByType(PropertyType type) const;
        [[nodiscard]] Property* GetPropertyByName(const std::string& name) const;
        [[nodiscard]] const std::string& GetName() const;

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

#ifdef EDITOR
        [[nodiscard]] bool CanBeRenamed() const;
#endif

    protected:
        Constructor constructor;
        std::shared_ptr<ObjectClass> parentClass;
        std::vector<Property> properties;
        bool canBeRenamed = false;
        std::string name;
    };


    template <>
    [[nodiscard]] bool ObjectClass::IsA<Object>() const;
}
