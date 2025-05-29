//
// Created by steph on 5/27/2025.
//

#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Property.h"

namespace Vox
{
    class Object;
    class ObjectClass;

    struct PropertyOverride
    {
        std::vector<std::string> path;
        PropertyType type;
        PropertyVariant overrideValue;
    };

    class Prefab
    {
    public:
        explicit Prefab(const std::string& filename);

        std::shared_ptr<Object> Construct();

    private:
        void CreateOverrides(const nlohmann::json& context, std::vector<std::string> currentPathStack);

        const ObjectClass* parent;

        std::vector<PropertyOverride> propertyOverrides;
        std::vector<Object> additionalObjects;
    };

} // Vox
