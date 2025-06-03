#pragma once
#include <functional>

#include "Property.h"

namespace Vox
{
    struct ObjectInitializer;
    class Object;

    class ObjectClass
    {
    public:
        using Constructor = std::function<std::shared_ptr<Object>(const ObjectInitializer&)>;
        ObjectClass(Constructor constructor, std::vector<Property> properties);
        virtual ~ObjectClass() = default;

        [[nodiscard]] Constructor GetConstructor() const;
        [[nodiscard]] ObjectClass* GetParentClass() const;
        [[nodiscard]] const std::vector<Property>& GetProperties() const;
        [[nodiscard]] Property* GetPropertyByType(PropertyType type) const;
        [[nodiscard]] Property* GetPropertyByName(const std::string& name) const;

#ifdef EDITOR
        [[nodiscard]] bool CanBeRenamed() const;
#endif

    protected:
        Constructor constructor;
        ObjectClass* parentClass;
        std::vector<Property> properties;
        bool canBeRenamed = false;
    };
}
