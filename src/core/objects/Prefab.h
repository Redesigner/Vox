//
// Created by steph on 5/27/2025.
//

#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ObjectClass.h"
#include "Property.h"

namespace Vox
{
    struct ObjectInitializer;
    class Object;
    class ObjectClass;

    struct PropertyOverride
    {
        std::vector<std::string> path;
        std::string propertyName;
        PropertyType type;
        PropertyVariant overrideValue;
    };

    struct PrefabContext
    {
        PrefabContext(const std::string& filename);

        void CreateOverrides(const nlohmann::json& context, std::vector<std::string> currentPathStack);

        const ObjectClass* parent;

        std::vector<PropertyOverride> propertyOverrides;
        std::vector<Object> additionalObjects;
    };

    class Prefab : public ObjectClass
    {
    public:
        explicit Prefab(const std::string& filename);

    private:
        static std::shared_ptr<Object> Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext);

        static void OverrideProperty(const std::shared_ptr<Object>& object, const PropertyOverride& propertyOverride);
    };

} // Vox
