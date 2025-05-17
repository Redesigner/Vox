#pragma once
#include <functional>

#include "Property.h"

namespace Vox
{
    class Object;

    class ObjectClass
    {
    public:
        using Constructor = std::function<std::shared_ptr<Object>()>;
        ObjectClass(Constructor constructor, std::vector<Property> properties);

        [[nodiscard]] std::function<std::shared_ptr<Object>()> GetConstructor() const;
        [[nodiscard]] const std::vector<Property>& GetProperties() const;
        [[nodiscard]] Property* GetPropertyByType(PropertyType type) const;
        
    private:
        std::function<std::shared_ptr<Object>()> constructor;
        std::vector<Property> properties;
    };
}
