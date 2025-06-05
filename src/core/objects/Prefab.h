//
// Created by steph on 5/27/2025.
//

#pragma once
#include <memory>
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

    struct AdditionalObject
    {
        std::weak_ptr<ObjectClass> objectClass;
        std::string objectName;
    };

    struct PrefabContext
    {
        explicit PrefabContext(const std::string& filename);
        explicit PrefabContext(const nlohmann::json& jsonObject);
        explicit PrefabContext(const Object* object);

        void CreateOverrides(const nlohmann::json& context, const std::vector<std::string>& currentPathStack);

        std::weak_ptr<ObjectClass> parent;

        std::vector<PropertyOverride> propertyOverrides;
        std::vector<AdditionalObject> additionalObjects;

    private:
        void CreateAdditionalObjects(const nlohmann::json& context);
    };

    class Prefab : public ObjectClass
    {
    public:
        explicit Prefab(const std::string& filename);
        explicit Prefab(const nlohmann::json& jsonObject);
        explicit Prefab(const Object* object);
        ~Prefab() override = default;

        void SaveToFile(const std::string& filename) const;
        [[nodiscard]] nlohmann::ordered_json Serialize() const;

    private:
        static std::shared_ptr<Object> Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext);

        static void OverrideProperty(const std::shared_ptr<Object>& object, const PropertyOverride& propertyOverride);

        std::shared_ptr<PrefabContext> context;
    };

} // Vox
