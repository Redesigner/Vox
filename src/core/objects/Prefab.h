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
#include "PropertyOverride.h"

namespace Vox
{
    struct ObjectInitializer;
    class Object;
    class ObjectClass;

    struct PrefabContext
    {
        explicit PrefabContext(const std::string& filename);
        explicit PrefabContext(const Object* object);

        void CreateOverrides(const nlohmann::json& context, std::vector<std::string> currentPathStack);

        const ObjectClass* parent;

        std::vector<PropertyOverride> propertyOverrides;
        std::vector<Object> additionalObjects;
    };

    class Prefab : public ObjectClass
    {
    public:
        explicit Prefab(const std::string& filename);
        explicit Prefab(const Object* object);
        ~Prefab() override = default;

    private:
        static std::shared_ptr<Object> Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext);

        static void OverrideProperty(const std::shared_ptr<Object>& object, const PropertyOverride& propertyOverride);
    };

} // Vox
